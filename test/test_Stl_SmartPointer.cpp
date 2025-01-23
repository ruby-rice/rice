#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <memory>

using namespace Rice;

TESTSUITE(SmartPointer);

namespace
{
  class MyClass
  {
  public:
    static inline int32_t constructorCalls = 0;
    static inline int32_t copyConstructorCalls = 0;
    static inline int32_t moveConstructorCalls = 0;
    static inline int32_t destructorCalls = 0;

    static void reset()
    {
      constructorCalls = 0;
      copyConstructorCalls = 0;
      moveConstructorCalls = 0;
      destructorCalls = 0;
    }

  public:
    int flag = 0;

  public:
    MyClass()
    {
      constructorCalls++;
    }

    ~MyClass()
    {
      destructorCalls++;
    }

    MyClass(const MyClass& other): flag(other.flag)
    {
      copyConstructorCalls++;
    }

    MyClass(MyClass&& other) : flag(other.flag)
    {
      moveConstructorCalls++;
    }

    void setFlag(int value)
    {
      this->flag = value;
    }
  };

  class Factory
  {
  public:
    static void reset()
    {
      Factory::instance_.reset();
    }

    ~Factory()
    {
    }

    std::unique_ptr<MyClass> transfer()
    {
      return std::make_unique<MyClass>();
    }

    std::shared_ptr<MyClass> share()
    {
      if (!instance_)
      {
        instance_ = std::make_shared<MyClass>();
      }
      return instance_;
    }

    std::shared_ptr<MyClass>& share_ref()
    {
      if (!instance_)
      {
        instance_ = std::make_shared<MyClass>();
      }
      return instance_;
    }

  public:
    static inline std::shared_ptr<MyClass> instance_;
  };

  class Sink
  {
  public:
    int takeOwnership(std::unique_ptr<MyClass> ptr)
    {
      return ptr->flag;
    }

    long shareOwnership(std::shared_ptr<MyClass> ptr)
    {
      return ptr.use_count();
    }

    int updatePointer(std::unique_ptr<MyClass>& ptr, MyClass* myClass)
    {
      ptr.reset(myClass);
      return ptr->flag;
    }

    int updatePointer(std::shared_ptr<MyClass>& ptr, MyClass* myClass)
    {
      ptr.reset(myClass);
      return ptr->flag;
    }

    std::shared_ptr<void> makeVoidShared(MyClass* myClass)
    {
      return std::shared_ptr<void>(myClass);
    }

    int handleVoid(std::shared_ptr<void>& ptr)
    {
      MyClass* myClass = static_cast<MyClass*>(ptr.get());
      return myClass->flag;
    }
  };

  int extractFlagUniquePtrRef(std::unique_ptr<MyClass>& myClass)
  {
    return myClass->flag;
  }

  int extractFlagSharedPtr(std::shared_ptr<MyClass> myClass)
  {
    return myClass->flag;
  }

  int extractFlagSharedPtrRef(std::shared_ptr<MyClass>& myClass)
  {
    return myClass->flag;
  }
}

SETUP(SmartPointer)
{
  embed_ruby();

#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 2
  rb_eval_string("GC.stress = true");
#endif

  define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("set_flag", &MyClass::setFlag);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_method("transfer", &Factory::transfer).
    define_method("share", &Factory::share).
    define_method("share_ref", &Factory::share_ref);

  define_class<Sink>("Sink").
    define_constructor(Constructor<Sink>()).
    define_method("take_ownership", &Sink::takeOwnership).
    define_method("share_ownership", &Sink::shareOwnership).
    define_method<int(Sink::*)(std::unique_ptr<MyClass>&, MyClass*)>("update_pointer", &Sink::updatePointer, 
      Arg("ptr"), Arg("myClass").takeOwnership()).
    define_method<int(Sink::*)(std::shared_ptr<MyClass>&, MyClass*)>("update_pointer", &Sink::updatePointer,
      Arg("ptr"), Arg("myClass").takeOwnership()).
    define_method("make_void_shared", &Sink::makeVoidShared,
      Arg("myClass").takeOwnership()).
    define_method<int(Sink::*)(std::shared_ptr<void>&)>("handle_void", &Sink::handleVoid);

  // Needed for shared_ptr<void>
  define_class<void>("Void");

  define_global_function("extract_flag_unique_ptr_ref", &extractFlagUniquePtrRef);
  define_global_function("extract_flag_shared_ptr", &extractFlagSharedPtr);
  define_global_function("extract_flag_shared_ptr_ref", &extractFlagSharedPtrRef);

  define_global_function("extract_flag_shared_ptr_with_default", &extractFlagSharedPtr,
      Arg("myClass") = std::make_shared<MyClass>());
  define_global_function("extract_flag_shared_ptr_ref_with_default", &extractFlagSharedPtrRef,
      Arg("myClass") = std::make_shared<MyClass>());
}

TEARDOWN(SmartPointer)
{
  rb_gc_start();
#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 2
  rb_eval_string("GC.stress = false");
#endif
}

TESTCASE(takeOwnership)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.transfer
                          my_class.set_flag(i)
                          my_class = nil
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(10, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(10, MyClass::destructorCalls);
  ASSERT(!Factory::instance_);
}

TESTCASE(ShareOwnership)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(ary = Array.new
                        factory = Factory.new
                        10.times do |i|
                          my_class = factory.share
                          my_class.set_flag(i)
                          ary << my_class
                        end)";

  ASSERT_EQUAL(0, Factory::instance_.use_count());
  m.module_eval(code);
  
  ASSERT_EQUAL(11, Factory::instance_.use_count());
  rb_gc_start();
  ASSERT_EQUAL(1, Factory::instance_.use_count());

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(0, MyClass::destructorCalls);
  ASSERT_EQUAL(9, Factory::instance_->flag);
}

TESTCASE(ShareOwnership2)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.share
                          my_class.set_flag(i)
                        end)";

  Factory factory;
  std::shared_ptr<MyClass> myClass = factory.share();
  ASSERT_EQUAL(2, Factory::instance_.use_count());

  // Call some ruby code
  Data_Object<Factory> wrapper(factory);
  ASSERT_EQUAL(2, Factory::instance_.use_count());
  wrapper.instance_eval("self.share_ref.set_flag(1)");

  ASSERT_EQUAL(3, Factory::instance_.use_count());
  rb_gc_start();
  ASSERT_EQUAL(2, Factory::instance_.use_count());
}

TESTCASE(UniquePtrRefParameter)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.transfer
                        my_class.set_flag(7)
                        extract_flag_unique_ptr_ref(my_class))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SharedPtrParameter)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(8)
                        extract_flag_shared_ptr(my_class))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(8, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SharedPtrRefParameter)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(9)
                        extract_flag_shared_ptr_ref(my_class))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SharedPtrDefaultParameter)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(7)
                        extract_flag_shared_ptr_with_default())";

  Object result = m.module_eval(code);
  // The default value kicks in and ignores any previous pointer
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result));
}

TESTCASE(SharedPtrRefDefaultParameter)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(7)
                        extract_flag_shared_ptr_ref_with_default())";

  Object result = m.module_eval(code);
  rb_gc_start();

  // The default value kicks in and ignores any previous pointer
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result));

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
 // ASSERT_EQUAL(1, MyClass::destructorCalls);
}

TESTCASE(UniquePtrRoundTrip)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        my_class = factory.transfer
                        my_class.set_flag(5)
                        
                        sink = Sink.new
                        sink.take_ownership(my_class))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result.value()));

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
 // ASSERT_EQUAL(1, MyClass::destructorCalls);
}

TESTCASE(UniquePtrUpdate)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        my_class1 = factory.transfer
                        my_class1.set_flag(5)
                        
                        my_class2 = MyClass.new
                        my_class2.set_flag(11)

                        sink = Sink.new
                        sink.update_pointer(my_class1, my_class2))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(11, detail::From_Ruby<long>().convert(result.value()));
}

TESTCASE(SharedPtrRoundTrip)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        
                        sink = Sink.new
                        sink.share_ownership(my_class))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(3, detail::From_Ruby<long>().convert(result.value()));

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
 // ASSERT_EQUAL(0, MyClass::destructorCalls);
}

TESTCASE(SharedPtrUpdate)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        my_class1 = factory.share
                        my_class1.set_flag(7)

                        my_class2 = MyClass.new
                        my_class2.set_flag(14)

                        sink = Sink.new
                        sink.update_pointer(my_class1, my_class2))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(14, detail::From_Ruby<long>().convert(result.value()));
}

TESTCASE(SharedPtrVoid)
{
  MyClass::reset();
  Factory::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(my_class = MyClass.new
                        my_class.set_flag(9)
                        sink = Sink.new
                        void_ptr = sink.make_void_shared(my_class)
                        sink.handle_void(void_ptr))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(9, detail::From_Ruby<long>().convert(result.value()));

  rb_gc_start();

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
 // ASSERT_EQUAL(1, MyClass::destructorCalls);
}
