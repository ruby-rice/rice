#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <memory>

using namespace Rice;

TESTSUITE(SharedPtr);

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
    std::string name = "default";
    int threshold = 0;

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

  class MyClassInherited : public MyClass
  {
  public:
    int extra = 0;

    void setExtra(int value)
    {
      this->extra = value;
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

    std::shared_ptr<MyClassInherited> share_inherited()
    {
      return std::make_shared<MyClassInherited>();
    }

  public:
    static inline std::shared_ptr<MyClass> instance_;
  };

  class Sink
  {
  public:
    long shareOwnership(std::shared_ptr<MyClass> ptr)
    {
      return ptr.use_count();
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

  int extractFlagSharedPtr(std::shared_ptr<MyClass> myClass)
  {
    return myClass->flag;
  }

  int extractFlagSharedPtrRef(std::shared_ptr<MyClass>& myClass)
  {
    return myClass->flag;
  }
}

SETUP(SharedPtr)
{
  embed_ruby();

#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 2
  rb_eval_string("GC.stress = true");
#endif

  define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("set_flag", &MyClass::setFlag).
    define_attr("name", &MyClass::name, Rice::AttrAccess::Read).
    define_attr("threshold", &MyClass::threshold);

  define_class<MyClassInherited, MyClass>("MyClassInherited").
    define_constructor(Constructor<MyClassInherited>()).
    define_method("set_extra", &MyClassInherited::setExtra).
    define_attr("extra", &MyClassInherited::extra);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_method("share", &Factory::share).
    define_method("share_ref", &Factory::share_ref).
    define_method("share_inherited", &Factory::share_inherited);

  define_class<Sink>("Sink").
    define_constructor(Constructor<Sink>()).
    define_method("share_ownership", &Sink::shareOwnership).
    define_method("update_pointer", &Sink::updatePointer,
      Arg("ptr"), Arg("myClass").takeOwnership()).
    define_method("make_void_shared", &Sink::makeVoidShared,
      Arg("myClass").takeOwnership()).
    define_method<int(Sink::*)(std::shared_ptr<void>&)>("handle_void", &Sink::handleVoid);

  define_global_function("extract_flag_shared_ptr", &extractFlagSharedPtr);
  define_global_function("extract_flag_shared_ptr_ref", &extractFlagSharedPtrRef);

  define_global_function("extract_flag_shared_ptr_with_default", &extractFlagSharedPtr,
      Arg("myClass") = std::make_shared<MyClass>());
  define_global_function("extract_flag_shared_ptr_ref_with_default", &extractFlagSharedPtrRef,
      Arg("myClass") = std::make_shared<MyClass>());
}

TEARDOWN(SharedPtr)
{
  rb_gc_start();
#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 2
  rb_eval_string("GC.stress = false");
#endif
}

TESTCASE(ShareOwnership)
{
  Factory::reset();
  MyClass::reset();

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
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.share
                          my_class.set_flag(i)
                        end)";

  ASSERT_EQUAL(0, Factory::instance_.use_count());
  m.module_eval(code);

#if RICE_RELEASE
  ASSERT_EQUAL(1, Factory::instance_.use_count());
#else
  ASSERT_EQUAL(11, Factory::instance_.use_count());
#endif
  rb_gc_start();
  ASSERT_EQUAL(1, Factory::instance_.use_count());

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(0, MyClass::destructorCalls);
  ASSERT_EQUAL(9, Factory::instance_->flag);
}

TESTCASE(PtrParameter)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(8)
                        extract_flag_shared_ptr(my_class))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(8, detail::From_Ruby<int>().convert(result));
}

TESTCASE(RefParameter)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(9)
                        extract_flag_shared_ptr_ref(my_class))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(result));
}

TESTCASE(DefaultParameter)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.share
                        my_class.set_flag(7)
                        extract_flag_shared_ptr_with_default())";

  Object result = m.module_eval(code);
  // The default value kicks in and ignores any previous pointer
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result));
}

TESTCASE(RefDefaultParameter)
{
  Factory::reset();
  MyClass::reset();

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
  ASSERT_EQUAL(0, MyClass::destructorCalls);
}

TESTCASE(RoundTrip)
{
  Factory::reset();
  MyClass::reset();

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
  ASSERT_EQUAL(0, MyClass::destructorCalls);
}

TESTCASE(Update)
{
  Factory::reset();
  MyClass::reset();

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

TESTCASE(Klass)
{
  define_shared_ptr<MyClass>(); 
  detail::TypeDetail<std::shared_ptr<MyClass>> typeDetail;
  Module aModule("Std");
  Object expected = aModule.const_get("SharedPtr≺AnonymousNamespace꞉꞉MyClass≻");
  VALUE actual = typeDetail.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
}

TESTCASE(KlassSharedPtr)
{
  define_shared_ptr<MyClass>();
  detail::TypeDetail<std::shared_ptr<MyClass>> typeDetail;

  Module stdModule("Std");
  Object expected = stdModule.const_get("SharedPtr≺AnonymousNamespace꞉꞉MyClass≻");
  VALUE actual = typeDetail.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
}

TESTCASE(Void)
{
  Factory::reset();
  MyClass::reset();

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

namespace
{
  std::shared_ptr<int> createPointer(int value)
  {
    int* sharedInt = new int(value);
    std::shared_ptr<int> shared(sharedInt);
    return shared;
  }

  const std::shared_ptr<int> createConstPointer(int value)
  {
    int* sharedInt = new int(value);
    const std::shared_ptr<int> shared(sharedInt);
    return shared;
  }

  int getPointerValue(std::shared_ptr<int> ptr)
  {
    return *ptr;
  }

  int getConstPointerValue(const std::shared_ptr<int>& ptr)
  {
    return *ptr;
  }

  int updatePointerValue(std::shared_ptr<int> ptr)
  {
    *ptr = *ptr + 1;
    return *ptr;
  }
}

TESTCASE(PointerToInt)
{
  Module m = define_module("SharedPtrInt").
    define_module_function("create_pointer", &createPointer).
    define_module_function("create_const_pointer", &createConstPointer).
    define_module_function("get_pointer_value", &getPointerValue).
    define_module_function("get_const_pointer_value", &getConstPointerValue);

  std::string code = R"(ptr = create_pointer(44)
                        get_pointer_value(ptr))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(44, detail::From_Ruby<int>().convert(result.value()));

  code = R"(ptr = create_const_pointer(45)
            get_const_pointer_value(ptr))";

  result = m.instance_eval(code);
  ASSERT_EQUAL(45, detail::From_Ruby<int>().convert(result.value()));

  code = R"(ptr = create_const_pointer(46)
            get_pointer_value(ptr))";

  result = m.instance_eval(code);
  ASSERT_EQUAL(46, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(CreatePointerToInt)
{
  Module m = define_module("CreatePointerToInt").
    define_module_function("get_pointer_value", &getPointerValue);

  std::string code = R"(buffer = Rice::Buffer≺int≻.new(45)
                        ptr = Std::SharedPtr≺int≻.new(buffer.release)
                        get_pointer_value(ptr))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(45, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(UpdatePointerToInt)
{
  Module m = define_module("UpdatePointerToInt").
    define_module_function("update_pointer_value", &updatePointerValue);

  std::string code = R"(buffer = Rice::Buffer≺int≻.new(45)
                        ptr = Std::SharedPtr≺int≻.new(buffer.release)
                        update_pointer_value(ptr))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(46, detail::From_Ruby<int>().convert(result.value()));
}

TESTCASE(ReadPointerToInt)
{
  Module m = define_module("ReadPointerToInt").
    define_module_function("create_pointer", &createPointer);

  std::string code = R"(ptr = create_pointer(50)
                        ptr.get.buffer.to_ary(1))";

  Array array = m.instance_eval(code);
  std::vector<int> actual = array.to_vector<int>();
  std::vector<int> expected{ 50 };
  ASSERT_EQUAL(expected, actual);
}

TESTCASE(AttributeForwarding)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  // Test read-only attribute
  std::string code = R"(factory = Factory.new
                        ptr = factory.share
                        ptr.name)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL("default", detail::From_Ruby<std::string>().convert(result));

  // Test read/write attribute - read
  code = R"(factory = Factory.new
            ptr = factory.share
            ptr.threshold)";

  result = m.module_eval(code);
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(result));

  // Test read/write attribute - write and read back
  code = R"(factory = Factory.new
            ptr = factory.share
            ptr.threshold = 42
            ptr.threshold)";

  result = m.module_eval(code);
  ASSERT_EQUAL(42, detail::From_Ruby<int>().convert(result));
}

TESTCASE(InheritedForwarding)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  // Test inherited method from MyClass via forwarding
  std::string code = R"(factory = Factory.new
                        $ptr = factory.share_inherited
                        $ptr.set_flag(99))";

  m.module_eval(code);

  // Verify via C++ that set_flag worked
  std::shared_ptr<MyClassInherited>* ptr = detail::From_Ruby<std::shared_ptr<MyClassInherited>*>().convert(
    m.module_eval("$ptr"));
  ASSERT_EQUAL(99, (*ptr)->flag);

  // Test inherited read-only attribute from MyClass
  code = R"(factory = Factory.new
            ptr = factory.share_inherited
            ptr.name)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL("default", detail::From_Ruby<std::string>().convert(result));

  // Test inherited read/write attribute from MyClass
  code = R"(factory = Factory.new
            ptr = factory.share_inherited
            ptr.threshold = 77
            ptr.threshold)";

  result = m.module_eval(code);
  ASSERT_EQUAL(77, detail::From_Ruby<int>().convert(result));

  // Test own method on MyClassInherited via forwarding
  code = R"(factory = Factory.new
            ptr = factory.share_inherited
            ptr.set_extra(123)
            ptr.extra)";

  result = m.module_eval(code);
  ASSERT_EQUAL(123, detail::From_Ruby<int>().convert(result));

  // Test own attribute on MyClassInherited via forwarding
  code = R"(factory = Factory.new
            ptr = factory.share_inherited
            ptr.extra = 456
            ptr.extra)";

  result = m.module_eval(code);
  ASSERT_EQUAL(456, detail::From_Ruby<int>().convert(result));
}

// Forward declaration only - IncompleteClass is never defined
class IncompleteClass;

namespace
{
  // Function that takes shared_ptr to incomplete type by value
  bool acceptIncompletePtr(std::shared_ptr<IncompleteClass> ptr)
  {
    return ptr == nullptr;
  }

  // Function that takes shared_ptr to incomplete type by const reference
  bool acceptIncompletePtrRef(const std::shared_ptr<IncompleteClass>& ptr)
  {
    return ptr == nullptr;
  }

  // Function that returns shared_ptr to incomplete type
  std::shared_ptr<IncompleteClass> returnIncompletePtr()
  {
    return nullptr;
  }
}

TESTCASE(IncompleteType)
{
  // This test verifies that std::shared_ptr<T> works with incomplete types.
  // The key is that Rice should NOT try to define a constructor taking T*
  // for incomplete types, since that would require T to be complete for deletion.

  // Register the incomplete type with Rice (no constructors since type is incomplete)
  define_class<IncompleteClass>("IncompleteClass");

  Module m = define_module("IncompleteTypeTest").
    define_module_function("accept_incomplete_ptr", &acceptIncompletePtr).
    define_module_function("accept_incomplete_ptr_ref", &acceptIncompletePtrRef).
    define_module_function("return_incomplete_ptr", &returnIncompletePtr);

  // Test returning nullptr shared_ptr of incomplete type
  std::string code = R"(ptr = return_incomplete_ptr
                        accept_incomplete_ptr(ptr))";

  Object result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());

  // Test with const ref parameter
  code = R"(ptr = return_incomplete_ptr
            accept_incomplete_ptr_ref(ptr))";

  result = m.instance_eval(code);
  ASSERT_EQUAL(Qtrue, result.value());
}