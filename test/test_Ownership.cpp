#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

#include <memory>

using namespace Rice;

TESTSUITE(Ownership);

namespace
{
  class MyClass
  {
  public:
    static inline int constructorCalls = 0;
    static inline int copyConstructorCalls = 0;
    static inline int moveConstructorCalls = 0;
    static inline int destructorCalls = 0;
    static inline int methodCalls = 0;

    static void reset()
    {
      constructorCalls = 0;
      copyConstructorCalls = 0;
      moveConstructorCalls = 0;
      destructorCalls = 0;
      methodCalls = 0;
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

    int32_t process()
    {
      methodCalls++;
      return methodCalls;
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
      delete Factory::instance_;
      Factory::instance_ = nullptr;
    }

  public:
    MyClass* transferPointerToRuby()
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
      return std::move(MyClass());
    }

    void transferPointerToCpp(MyClass* myClass)
    {
      delete myClass;
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

SETUP(Ownership)
{
  embed_ruby();

  define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass>()).
    define_method("process", &MyClass::process).
    define_method("set_flag", &MyClass::setFlag);

  define_class<Factory>("Factory").
    define_constructor(Constructor<Factory>()).
    define_method("value", &Factory::value).
    define_method("move_value", &Factory::moveValue).
    define_method("transfer_pointer_to_ruby", &Factory::transferPointerToRuby, Return().takeOwnership()).
    define_method("transfer_pointer_to_cpp", &Factory::transferPointerToCpp, Arg("myClass").takeOwnership()).
    define_method("keep_pointer", &Factory::keepPointer).
    define_method("copy_reference", &Factory::keepReference, Return().takeOwnership()).
    define_method("keep_reference", &Factory::keepReference);
}

TEARDOWN(Ownership)
{
  rb_gc_start();
}

TESTCASE(TransferPointerToRuby)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.transfer_pointer_to_ruby
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

TESTCASE(TransferPointerToCpp)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(myClass = MyClass.new
                        factory = Factory.new
                        factory.transfer_pointer_to_cpp(myClass))";

  m.module_eval(code);
  rb_gc_start();

  ASSERT(true);
}

TESTCASE(KeepPointer)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.keep_pointer
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(0, MyClass::destructorCalls);
  ASSERT_EQUAL(9, Factory::instance_->flag);
}

TESTCASE(KeepReference)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.keep_reference
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(0, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(0, MyClass::destructorCalls);
  ASSERT_EQUAL(9, Factory::instance_->flag);
}

TESTCASE(CopyReference)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  // Create ruby objects that point to the same instance of MyClass
  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.copy_reference
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(1, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(10, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(10, MyClass::destructorCalls);
  ASSERT_EQUAL(0, Factory::instance_->flag);
}

TESTCASE(TransferValue)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.value
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(10, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(10, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(20, MyClass::destructorCalls);
  ASSERT(!Factory::instance_);
}

TESTCASE(MoveValue)
{
  Factory::reset();
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        10.times do |i|
                          my_class = factory.move_value
                          my_class.set_flag(i)
                        end)";

  m.module_eval(code);
  rb_gc_start();

  ASSERT_EQUAL(10, MyClass::constructorCalls);
  ASSERT_EQUAL(0, MyClass::copyConstructorCalls);
  ASSERT_EQUAL(20, MyClass::moveConstructorCalls);
  ASSERT_EQUAL(30, MyClass::destructorCalls);
  ASSERT(!Factory::instance_);
}

namespace
{
  class MyClassNotCopyable
  {
  public:
    MyClassNotCopyable() = default;
    MyClassNotCopyable(const MyClassNotCopyable& other) = delete;
    MyClassNotCopyable(MyClassNotCopyable&& other) = delete;
    int value;
  };
}

TESTCASE(NotCopyable)
{
  Class c = define_class<MyClassNotCopyable>("MyClassNotCopyable")
    .define_constructor(Constructor<MyClassNotCopyable>())
    .define_attr("value", &MyClassNotCopyable::value);

  MyClassNotCopyable instance;

#ifdef _MSC_VER
  const char* expected = "Ruby was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: class `anonymous namespace'::MyClassNotCopyable";
#else
  const char* expected = "Ruby was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: (anonymous namespace)::MyClassNotCopyable";
#endif

  // Trying to take ownership should fail
  ASSERT_EXCEPTION_CHECK(
    std::runtime_error,
    Data_Object<MyClassNotCopyable>(instance, true),
    ASSERT_EQUAL(expected, ex.what())
  );
}
