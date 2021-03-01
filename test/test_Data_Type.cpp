#include <assert.h> 

#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Data_Type);

SETUP(Data_Type)
{
  embed_ruby();
}

/**
 * The tests here are for the feature of taking an instance
 * of a Ruby-subclass of a Rice wrapped class and passing
 * that instance back into the Rice wrapper. While that
 * might be confusing, the test code is pretty straight foward
 * to see what we're talking about.
 */

namespace
{
  class MyClass
  {
  public:
    static inline bool no_return_no_arg_called = false;
    static inline bool no_arg_called = false;
    static inline bool int_arg_called = false;
    static inline bool multiple_args_called = false;

    static void reset()
    {
      no_return_no_arg_called = false;
      no_arg_called = false;
      int_arg_called = false;
      multiple_args_called = false;
    }

    static Object singleton_method_object_int(Object object, int anInt)
    {
      return object;
    }

    static int singleton_function_int(int anInt)
    {
      return anInt;
    }

  public:
    MyClass() = default;
    MyClass(const MyClass& other) = delete;
    MyClass(MyClass&& other) = delete;

    void no_return_no_arg()
    {
      no_return_no_arg_called = true;
    }

    bool no_arg()
    {
      no_arg_called = true;
      return true;
    }

    int int_arg(int i)
    {
      int_arg_called = true;
      return i;
    }

    std::string multiple_args(int i, bool b, float f, std::string s)
    {
      multiple_args_called = true;
      return "multiple_args(" + std::to_string(i) + ", " + std::to_string(b) + ", " +
        std::to_string(f) + ", " + s + ")";
    }

  public:
    const char readOnly[13] = "Read a char!";
    int writeOnly = 0;
    std::string readWrite;
  };
} // namespace

TESTCASE(methods_with_member_pointers)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("no_return_no_arg", &MyClass::no_return_no_arg)
    .define_method("no_arg", &MyClass::no_arg)
    .define_method("int_arg", &MyClass::int_arg)
    .define_method("multiple_args", &MyClass::multiple_args);

  MyClass::reset();
  Object o = c.call("new");

  Object result = o.call("no_return_no_arg");
  ASSERT(MyClass::no_return_no_arg_called);
  ASSERT_EQUAL(Qnil, result.value());

  result = o.call("no_arg");
  ASSERT(MyClass::no_arg_called);
  ASSERT_EQUAL(Qtrue, result.value());

  result = o.call("int_arg", 42);
  ASSERT(MyClass::int_arg_called);
  ASSERT_EQUAL(42, detail::From_Ruby<int>::convert(result.value()));

  result = o.call("multiple_args", 81, true, 7.0, "a string");
  ASSERT(MyClass::multiple_args_called);
  ASSERT_EQUAL("multiple_args(81, 1, 7.000000, a string)", detail::From_Ruby<std::string>::convert(result.value()));
}

TESTCASE(incorrect_number_of_args)
{
  Class c =
    define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("int_arg", &MyClass::int_arg);

  Object o = c.call("new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("int_arg", 1, 2),
    ASSERT_EQUAL(rb_eArgError, ex.class_of())
  );
}

TESTCASE(incorrect_no_args)
{
  Class c =
    define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("int_arg", &MyClass::int_arg);

  Object o = c.call("new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("int_arg"),
    ASSERT_EQUAL(rb_eArgError, ex.class_of())
  );
}

TESTCASE(methods_with_lambdas)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_method("no_return_no_arg", 
        [](MyClass& instance)
        {
          instance.no_return_no_arg();
        })
    .define_method("no_arg",
        [](MyClass& instance)
        {
          return instance.no_arg();
        })
    .define_method("int_arg", 
        [](MyClass& instance, int anInt)
        {
          return instance.int_arg(anInt);
        })
    .define_method("multiple_args",
        [](MyClass& instance, int anInt, bool aBool, float aFloat, std::string aString)
        {
          return instance.multiple_args(anInt, aBool, aFloat, aString);
        });

  MyClass::reset();
  Object o = c.call("new");

  Object result = o.call("no_return_no_arg");
  ASSERT(MyClass::no_return_no_arg_called);
  ASSERT_EQUAL(Qnil, result.value());

  result = o.call("no_arg");
  ASSERT(MyClass::no_arg_called);
  ASSERT_EQUAL(Qtrue, result.value());

  result = o.call("int_arg", 42);
  ASSERT(MyClass::int_arg_called);
  ASSERT_EQUAL(42, detail::From_Ruby<int>::convert(result.value()));

  result = o.call("multiple_args", 81, true, 7.0, "a string");
  ASSERT(MyClass::multiple_args_called);
  ASSERT_EQUAL("multiple_args(81, 1, 7.000000, a string)", detail::From_Ruby<std::string>::convert(result.value()));
}

TESTCASE(static_singleton_method)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_method("singleton_method_object_int", &MyClass::singleton_method_object_int);

  MyClass::reset();

  Object result = c.call("singleton_method_object_int", 42);
  ASSERT_EQUAL(c, result);
}

TESTCASE(static_singleton_function)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_function("singleton_function_int", &MyClass::singleton_function_int);

  MyClass::reset();

  Object result = c.call("singleton_function_int", 42);
  ASSERT_EQUAL(42, detail::From_Ruby<int>::convert(result));
}

TESTCASE(static_singleton_method_lambda)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_method("singleton_method_object_int", [](Object object, int anInt)
      {
        return MyClass::singleton_method_object_int(object, anInt);
      });

  MyClass::reset();

  Object result = c.call("singleton_method_object_int", 42);
  ASSERT_EQUAL(c, result);
}

TESTCASE(static_singleton_function_lambda)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_function("singleton_function_int", [](int anInt)
      {
        return MyClass::singleton_function_int(anInt);
      });

  MyClass::reset();

  Object result = c.call("singleton_function_int", 42);
  ASSERT_EQUAL(42, detail::From_Ruby<int>::convert(result));
}

TESTCASE(attributes)
{
  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_attr("read_char", &MyClass::readOnly, Rice::AttrAccess::Read)
    .define_attr("write_int", &MyClass::writeOnly, Rice::AttrAccess::Write)
    .define_attr("read_write_string", &MyClass::readWrite);

  Object o = c.call("new");
  MyClass* myClass = detail::From_Ruby<MyClass*>::convert(o);

  // Test readonly attribute
  Object result = o.call("read_char");
  ASSERT_EQUAL("Read a char!", detail::From_Ruby<char*>::convert(result));
  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("read_char=", "some text"),
  );
  
  // Test writeonly attribute
  result = o.call("write_int=", 5);
  ASSERT_EQUAL(5, detail::From_Ruby<int>::convert(result.value()));
  ASSERT_EQUAL(5, myClass->writeOnly);
  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("write_int", 3),
    );

  // Test readwrite attribute
  result = o.call("read_write_string=", "Set a string");
  ASSERT_EQUAL("Set a string", detail::From_Ruby<std::string>::convert(result.value()));
  ASSERT_EQUAL("Set a string", myClass->readWrite);

  result = o.call("read_write_string");
  ASSERT_EQUAL("Set a string", detail::From_Ruby<std::string>::convert(result.value()));
}

TESTCASE(static_attributes)
{
  MyClass::reset();

  Class c = define_class<MyClass>("MyClass")
    .define_constructor(Constructor<MyClass>())
    .define_singleton_attr("no_arg_called", &MyClass::no_arg_called, Rice::AttrAccess::Read)
    .define_singleton_attr("int_arg_called", &MyClass::int_arg_called, Rice::AttrAccess::Write)
    .define_singleton_attr("multiple_args_called", &MyClass::multiple_args_called);

  // Test readonly attribute
  Object result = c.call("no_arg_called");
  ASSERT_EQUAL(Qfalse, result.value());
  ASSERT_EXCEPTION_CHECK(
    Exception,
    c.call("no_arg_called=", true),
  );

  // Test writeonly attribute
  result = c.call("int_arg_called=", true);
  ASSERT_EQUAL(Qtrue, result.value());
  ASSERT_EQUAL(true, MyClass::int_arg_called);
  ASSERT_EXCEPTION_CHECK(
    Exception,
    c.call("int_arg_called"),
  );

  // Test readwrite attribute
  result = c.call("multiple_args_called=", true);
  ASSERT_EQUAL(Qtrue, result.value());
  ASSERT_EQUAL(true, MyClass::multiple_args_called);

  result = c.call("multiple_args_called");
  ASSERT_EQUAL(Qtrue, result.value());
}

namespace {
  class BaseClass
  {
  public:
    BaseClass() {}
  };
}

TESTCASE(subclassing)
{
  Module m = define_module("Testing");
  define_class_under<BaseClass>(m, "BaseClass").
    define_constructor(Constructor<BaseClass>());

  // Not sure how to make this a true failure case. If the subclassing
  // doesn't work, Ruby will throw an error:
  //
  //    in `new': wrong instance allocation
  //
  m.instance_eval("class NewClass < Testing::BaseClass; end;");
  m.instance_eval("n = NewClass.new");
}

namespace {
  float with_reference_defaults_x;
  std::string with_reference_defaults_str;

  class DefaultArgsRefs
  {
  public:
    void with_reference_defaults(float x, std::string const& str = std::string("testing"))
    {
      with_reference_defaults_x = x;
      with_reference_defaults_str = str;
    }
  };

}

TESTCASE(define_method_works_with_reference_const_default_values)
{
  Class c = define_class<DefaultArgsRefs>("DefaultArgsRefs")
    .define_constructor(Constructor<DefaultArgsRefs>())
    .define_method("bar",
      &DefaultArgsRefs::with_reference_defaults,
      Arg("x"), Arg("str") = std::string("testing"));

  Object o = c.call("new");
  o.call("bar", 3);

  ASSERT_EQUAL(3, with_reference_defaults_x);
  ASSERT_EQUAL("testing", with_reference_defaults_str);
}

namespace
{
  class RefTest
  {
  public:
    RefTest() {}

    static std::string& getReference()
    {
      static std::string foo = "foo";
      return foo;
    }
  };
}

TESTCASE(define_singleton_method_returning_reference)
{
  Class c = define_class<RefTest>("RefTest")
    .define_constructor(Constructor<RefTest>())
    .define_singleton_function("get_reference", &RefTest::getReference);

  Module m(anonymous_module());

  Object result = m.instance_eval("RefTest.get_reference");
  ASSERT_EQUAL(result, String("foo"));
}

namespace
{
  struct MyStruct
  {
    MyStruct* set(MyStruct* ptr)
    {
      assert(ptr == nullptr);
      return ptr;
    }

    MyStruct* get()
    {
      return nullptr;
    }
  };
}

TESTCASE(null_ptrs)
{
  Class c = define_class<MyStruct>("MyStruct")
    .define_constructor(Constructor<MyStruct>())
    .define_method("get", &MyStruct::get)
    .define_method("set", &MyStruct::set);

  Object o = c.call("new");

  Object result = o.call("get");
  ASSERT_EQUAL(Qnil, result.value());

  result = o.call("set", nullptr);
  ASSERT_EQUAL(Qnil, result.value());
}

/**
 * The following test SEGFAULTs right now
 */
/*
TESTCASE(no_super_in_constructor_still_works)
{
  Module m = define_module("TestingModule");
  Object handler = m.instance_eval("@handler = ListenerHandler.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  // Because of this, there's a constructor but no super call
  m.instance_eval("class MyListener < Listener; def initialize; @val = 10; end; end;");
  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(2), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(8), handler.call("process").value());
}
*/