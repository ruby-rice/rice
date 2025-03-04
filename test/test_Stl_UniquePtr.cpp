#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <memory>

using namespace Rice;

TESTSUITE(UniquePtr);

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
    std::unique_ptr<MyClass> transfer()
    {
      return std::make_unique<MyClass>();
    }
  };

  class Sink
  {
  public:
    int takeOwnership(std::unique_ptr<MyClass> ptr)
    {
      return ptr->flag;
    }

    int updatePointer(std::unique_ptr<MyClass>& ptr, MyClass* myClass)
    {
      ptr.reset(myClass);
      return ptr->flag;
    }
  };

  int extractFlagUniquePtrRef(std::unique_ptr<MyClass>& myClass)
  {
    return myClass->flag;
  }
}

SETUP(UniquePtr)
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
    define_method("transfer", &Factory::transfer);

  define_class<Sink>("Sink").
    define_constructor(Constructor<Sink>()).
    define_method("take_ownership", &Sink::takeOwnership).
    define_method("update_pointer", &Sink::updatePointer,
      Arg("ptr"), Arg("myClass").takeOwnership());

  define_global_function("extract_flag_unique_ptr_ref", &extractFlagUniquePtrRef);
}

TEARDOWN(UniquePtr)
{
  rb_gc_start();
#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR >= 2
  rb_eval_string("GC.stress = false");
#endif
}

TESTCASE(takeOwnership)
{
  MyClass::reset();

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
}

TESTCASE(RefParameter)
{
  MyClass::reset();

  Module m = define_module("TestingModule");

  std::string code = R"(factory = Factory.new
                        my_class = factory.transfer
                        my_class.set_flag(7)
                        extract_flag_unique_ptr_ref(my_class))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(result));
}

TESTCASE(RoundTrip)
{
  MyClass::reset();

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
  ASSERT_EQUAL(1, MyClass::destructorCalls);
}

TESTCASE(Update)
{
  MyClass::reset();

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