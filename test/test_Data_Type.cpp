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

  int define_method_int_result;

  class IntHelper
  {
  public:
    IntHelper() {}

    void define_method_int_helper(int i)
    {
      define_method_int_result = i;
    }
  };

} // namespace

TESTCASE(define_method_int)
{
  Class c =
    define_class<IntHelper>("IntHelper")
    .define_constructor(Constructor<IntHelper>())
    .define_method("foo", &IntHelper::define_method_int_helper);

  Object o = c.call("new");
  define_method_int_result = 0;
  o.call("foo", 42);
  ASSERT_EQUAL(42, define_method_int_result);
}

TESTCASE(define_method_int_passed_two_args)
{
  Class c =
    define_class<IntHelper>("IntHelper")
    .define_constructor(Constructor<IntHelper>())
    .define_method("foo", &IntHelper::define_method_int_helper);

  Object o = c.call("new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("foo", 1, 2),
    ASSERT_EQUAL(rb_eArgError, ex.class_of())
  );
}

TESTCASE(define_method_int_passed_no_args)
{
  Class c =
    define_class<IntHelper>("IntHelper")
    .define_constructor(Constructor<IntHelper>())
    .define_method("foo", &IntHelper::define_method_int_helper);

  Object o = c.call("new");

  ASSERT_EXCEPTION_CHECK(
    Exception,
    o.call("foo"),
    ASSERT_EQUAL(rb_eArgError, ex.class_of())
  );
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
    .define_singleton_method("get_reference", &RefTest::getReference);

  Module m(anonymous_module());

  Object result = m.instance_eval("RefTest.get_reference");
  ASSERT_EQUAL(result, String("foo"));
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
