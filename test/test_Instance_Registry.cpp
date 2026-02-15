#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Instance_Registry);

namespace
{
  class MyClass
  {
  public:
    int flag()
    {
      return flag_;
    }

  private:
    int flag_ = 123;
  };

  class Factory
  {
  public:
    static void reset()
    {
      delete Factory::instance_;
      Factory::instance_ = nullptr;
    }

  public:
    Factory* factory()
    {
      return this;
    }

    MyClass* transferPointer()
    {
      return new MyClass();
    }

    MyClass* keepPointer()
    {
      return this->instance();
    }

    MyClass& keepReference()
    {
      return *this->instance();
    }

    MyClass value()
    {
      return MyClass();
    }

    MyClass moveValue()
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
      return std::move(MyClass());
#pragma GCC diagnostic pop
    }

    MyClass* instance()
    {
      if (!instance_)
      {
        instance_ = new MyClass();
      }
      return instance_;
    }

  public:
    static inline MyClass* instance_ = nullptr;
  };
}

SETUP(Instance_Registry)
{
  embed_ruby();

  define_class<MyClass>("MyClass")
    .define_method("flag", &MyClass::flag);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_singleton_function("reset", &Factory::reset).
    define_method("factory", &Factory::factory).
    define_method("value", &Factory::value).
    define_method("move_value", &Factory::moveValue).
    define_method("transfer_pointer", &Factory::transferPointer, Return().takeOwnership()).
    define_method("keep_pointer", &Factory::keepPointer).
    define_method("copy_reference", &Factory::keepReference, Return().takeOwnership()).
    define_method("keep_reference", &Factory::keepReference);
}

TEARDOWN(Instance_Registry)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  rb_gc_start();
}

TESTCASE(TransferPointer)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  Factory::reset();

  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("transfer_pointer");
  Data_Object<MyClass> my_class2 = factory.call("transfer_pointer");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_NOT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(KeepPointerOwnedMode)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  Factory::reset();

  Module m = define_module("TestingModule");

  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("keep_pointer");
  Data_Object<MyClass> my_class2 = factory.call("keep_pointer");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(KeepPointerAllMode)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::All;
  Factory::reset();

  Module m = define_module("TestingModule");

  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("keep_pointer");
  Data_Object<MyClass> my_class2 = factory.call("keep_pointer");

  ASSERT(my_class1.is_equal(my_class2));
  ASSERT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(KeepPointerWithoutTracking)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Off;
  Factory::reset();

  Module m = define_module("TestingModule");

  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("keep_pointer");
  Data_Object<MyClass> my_class2 = factory.call("keep_pointer");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(KeepReferenceOwnedMode)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  Factory::reset();

  Module m = define_module("TestingModule");

  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("keep_reference");
  Data_Object<MyClass> my_class2 = factory.call("keep_reference");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(KeepReferenceAllMode)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::All;
  Factory::reset();

  Module m = define_module("TestingModule");

  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("keep_reference");
  Data_Object<MyClass> my_class2 = factory.call("keep_reference");

  ASSERT(my_class1.is_equal(my_class2));
  ASSERT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(KeepReferenceWithoutTracking)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Off;
  Factory::reset();

  Module m = define_module("TestingModule");

  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("keep_reference");
  Data_Object<MyClass> my_class2 = factory.call("keep_reference");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(CopyReference)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  Factory::reset();

  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("copy_reference");
  Data_Object<MyClass> my_class2 = factory.call("copy_reference");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_NOT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(TransferValue)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  Factory::reset();

  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("value");
  Data_Object<MyClass> my_class2 = factory.call("value");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_NOT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(MoveValue)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::Owned;
  Factory::reset();

  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");

  Data_Object<MyClass> my_class1 = factory.call("move_value");
  Data_Object<MyClass> my_class2 = factory.call("move_value");

  ASSERT(!my_class1.is_equal(my_class2));
  ASSERT_NOT_EQUAL(my_class1.get(), my_class2.get());
}

TESTCASE(RubyObjectGced)
{
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::All;
  Factory::reset();

  Module m = define_module("TestingModule");
  Object factory = m.module_eval("Factory.new");

  {
    // Track the C++ object returned by keepPointer
    Data_Object<MyClass> my_class1 = factory.call("keep_pointer");
    rb_gc_start();
  }

  // Make my_class1 invalid
  rb_gc_start();

  // Get the object again - this should *not* return the previous value
  Data_Object<MyClass> my_class2 = factory.call("keep_pointer");

  // Call a method on the ruby object
  String className = my_class2.class_name();
  ASSERT_EQUAL(std::string("MyClass"), className.str());
}

TESTCASE(AllMode_CppFreeUnderRubyWrapper_CrashRepro)
{
  Factory::reset();

  Module m = define_module("TestingModule");
  std::string code = R"(
    factory = Factory.new
    obj = factory.keep_pointer
    # Simulate C++ freeing the object out from under Ruby by resetting the Factory, which deletes the MyClass instance.
    Factory.reset
    obj.flag
  )";

  // C++ frees the object out from under Ruby while Ruby still holds a wrapper.
  detail::Registries::instance.instances.mode = detail::InstanceRegistry::Mode::All;
  Object result = m.module_eval(code);
  ASSERT_NOT_EQUAL(99, detail::From_Ruby<int>().convert(result.value()));
}
