#include "unittest.hpp"
#include "rice/Module.hpp"
#include "rice/Exception.hpp"
#include "rice/Array.hpp"
#include "rice/Arg.hpp"
#include "rice/global_function.hpp"
#include "rice/Constructor.hpp"

using namespace Rice;

TESTSUITE(Module);

SETUP(Object)
{
  ruby_init();
}

TESTCASE(construct_from_value)
{
  Module m(rb_mEnumerable);
  ASSERT_EQUAL(rb_mEnumerable, m.value());
}

namespace
{

class Quite_Silly_Exception
  : public std::exception
{
};

void handle_silly_exception(Quite_Silly_Exception const & ex)
{
  throw Exception(rb_eRuntimeError, "SILLY");
}

void throw_silly_exception(Object self)
{
  throw Quite_Silly_Exception();
}

} // namespace

TESTCASE(add_handler)
{
  Module m(anonymous_module());
  m.add_handler<Quite_Silly_Exception>(handle_silly_exception);
  m.define_singleton_method("foo", throw_silly_exception);
  Object exc = m.instance_eval("begin; foo; rescue Exception; $!; end");
  ASSERT_EQUAL(rb_eRuntimeError, CLASS_OF(exc));
  Exception ex(exc);
  ASSERT_EQUAL(String("SILLY"), String(ex.message()));
}

namespace
{

bool define_method_simple_ok;

void define_method_simple_helper(Object o)
{
  define_method_simple_ok = true;
}

} // namespace

TESTCASE(define_method_simple)
{
  Module m(anonymous_module());
  m.define_method("foo", define_method_simple_helper);
  define_method_simple_ok = false;
  Object o = m.instance_eval("o = Object.new; o.extend(self); o.foo");
  ASSERT(define_method_simple_ok);
}

TESTCASE(define_singleton_method_simple)
{
  Module m(anonymous_module());
  m.define_singleton_method("foo", define_method_simple_helper);
  define_method_simple_ok = false;
  Object o = m.call("foo");
  ASSERT(define_method_simple_ok);
}

TESTCASE(define_module_function_simple)
{
  Module m(anonymous_module());
  m.define_module_function("foo", define_method_simple_helper);
  define_method_simple_ok = false;
  m.instance_eval("o = Object.new; o.extend(self); o.instance_eval { foo }");
  ASSERT(define_method_simple_ok);
  define_method_simple_ok = false;
  m.call("foo");
  ASSERT(define_method_simple_ok);
}

namespace
{

int define_method_int_result;

void define_method_int_helper(Object o, int i)
{
  define_method_int_result = i;
}

} // namespace

TESTCASE(define_method_int)
{
  Module m(anonymous_module());
  m.define_method("foo", define_method_int_helper);
  define_method_int_result = 0;
  Object o = m.instance_eval("o = Object.new; o.extend(self); o.foo(42)");
  ASSERT_EQUAL(42, define_method_int_result);
}

TESTCASE(define_singleton_method_int)
{
  Module m(anonymous_module());
  m.define_singleton_method("foo", define_method_int_helper);
  define_method_int_result = 0;
  Object o = m.call("foo", 42);
  ASSERT_EQUAL(42, define_method_int_result);
}

TESTCASE(define_module_function_int)
{
  Module m(anonymous_module());
  m.define_module_function("foo", define_method_int_helper);
  define_method_int_result = 0;
  m.instance_eval("o = Object.new; o.extend(self); o.instance_eval { foo(42) }");
  ASSERT_EQUAL(42, define_method_int_result);
  define_method_int_result = 0;
  m.call("foo", 42);
  ASSERT_EQUAL(42, define_method_int_result);
}

TESTCASE(define_method_int_passed_no_args)
{
  Module m(anonymous_module());
  m.define_method("foo", define_method_int_helper);
  ASSERT_EXCEPTION_CHECK(
      Exception,
      m.instance_eval("o = Object.new; o.extend(self); o.foo"),
      ASSERT_EQUAL(
          Object(rb_eArgError),
          Object(CLASS_OF(ex.value()))
          )
      );
}

namespace
{

struct Foo
{
  int x;
};

int define_method_int_foo_result_i;
Foo * define_method_int_foo_result_x;

void define_method_int_foo_helper(Object o, int i, Foo * x)
{
  define_method_int_foo_result_i = i;
  define_method_int_foo_result_x = x;
}

} // namespace

template<>
Foo * from_ruby<Foo *>(Object x)
{
  Foo * retval;
  Data_Get_Struct(x.value(), Foo, retval);
  return retval;
}

TESTCASE(define_singleton_method_int_foo)
{
  Module m(anonymous_module());
  m.define_singleton_method("int_and_foo", define_method_int_foo_helper);
  define_method_int_result = 0;
  Foo * foo = new Foo;
  foo->x = 1024;
  VALUE f = Data_Wrap_Struct(rb_cObject, 0, Default_Allocation_Strategy<Foo>::free, foo);
  m.call("int_and_foo", 42, Object(f));
  ASSERT_EQUAL(42, define_method_int_foo_result_i);
  ASSERT_EQUAL(foo, define_method_int_foo_result_x);
}

// TODO: how to test define_iterator?

TESTCASE(include_module)
{
  Module m(anonymous_module());
  Module & m2(m.include_module(rb_mEnumerable));
  ASSERT_EQUAL(&m, &m2);
  Array ancestors(m.ancestors());
  Array expected_ancestors;
  expected_ancestors.push(m);
  expected_ancestors.push(Module(rb_mEnumerable));
  ASSERT_EQUAL(expected_ancestors, ancestors);
}

TESTCASE(const_set_get_by_id)
{
  Module m(anonymous_module());
  Object v = to_ruby(42);
  Module & m2(m.const_set(rb_intern("FOO"), v));
  ASSERT_EQUAL(&m, &m2);
  ASSERT_EQUAL(v, m.const_get(rb_intern("FOO")));
}

TESTCASE(const_set_get_by_identifier)
{
  Module m(anonymous_module());
  Object v = to_ruby(42);
  Module & m2(m.const_set(Identifier("FOO"), v));
  ASSERT_EQUAL(&m, &m2);
  ASSERT_EQUAL(v, m.const_get(Identifier("FOO")));
}

TESTCASE(const_set_get_by_string)
{
  Module m(anonymous_module());
  Object v = to_ruby(42);
  Module & m2(m.const_set("FOO", v));
  ASSERT_EQUAL(&m, &m2);
  ASSERT_EQUAL(v, m.const_get("FOO"));
}

TESTCASE(remove_const)
{
  Module m(anonymous_module());
  Object v = to_ruby(42);
  m.const_set("FOO", v);
  ASSERT_EQUAL(v, m.const_get("FOO"));
  m.remove_const("FOO");
  ASSERT_EXCEPTION_CHECK(
      Exception,
      m.const_get("FOO"),
      ASSERT_EQUAL(
          Object(rb_eNameError),
          Object(CLASS_OF(ex.value()))
          )
      );
}

TESTCASE(mod_name_anonymous)
{
  Module m(anonymous_module());
  ASSERT_EQUAL(String(""), m.name());
}

/**
 * Tests for default arguments
 */
namespace
{
  int defaults_method_one_arg1;
  int defaults_method_one_arg2;
  bool defaults_method_one_arg3 = false;

  void defaults_method_one(int arg1, int arg2 = 3, bool arg3 = true)
  {
    defaults_method_one_arg1 = arg1;
    defaults_method_one_arg2 = arg2;
    defaults_method_one_arg3 = arg3;
  }
}

TESTCASE(define_method_default_arguments)
{
  Module m(anonymous_module());
  m.define_method("foo", &defaults_method_one, (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true));

  Object o = m.instance_eval("o = Object.new; o.extend(self); o");
  o.call("foo", 2);

  ASSERT_EQUAL(2, defaults_method_one_arg1);
  ASSERT_EQUAL(3, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  o.call("foo", 11, 10);

  ASSERT_EQUAL(11, defaults_method_one_arg1);
  ASSERT_EQUAL(10, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  o.call("foo", 22, 33, false);

  ASSERT_EQUAL(22, defaults_method_one_arg1);
  ASSERT_EQUAL(33, defaults_method_one_arg2);
  ASSERT(!defaults_method_one_arg3);
}

TESTCASE(default_arguments_still_throws_argument_error)
{
  Module m(anonymous_module());
  m.define_method("foo", &defaults_method_one, (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true));

  ASSERT_EXCEPTION_CHECK(
      Exception,
      m.instance_eval("o = Object.new; o.extend(self); o.foo()"),
      ASSERT_EQUAL(
          Object(rb_eArgError),
          Object(CLASS_OF(ex.value()))
          )
      );

  ASSERT_EXCEPTION_CHECK(
      Exception,
      m.instance_eval("o = Object.new; o.extend(self); o.foo(3, 4, false, 17)"),
      ASSERT_EQUAL(
          Object(rb_eArgError),
          Object(CLASS_OF(ex.value()))
          )
      );
}

namespace {
  int the_one_default_arg = 0;
  void method_with_one_default_arg(int num = 4) {
    the_one_default_arg = num;
  }
}

TESTCASE(defining_methods_with_single_default_argument)
{
  // define_method
  Module m(anonymous_module());
  m.define_method("foo", &method_with_one_default_arg, (Arg("num") = 4));
  m.instance_eval("o = Object.new; o.extend(self); o.foo()");
  ASSERT_EQUAL(4, the_one_default_arg);

  the_one_default_arg = 0;

  // define_singleton_method
  Class c(anonymous_class());
  c.define_singleton_method("foo", &method_with_one_default_arg, (Arg("num") = 4));
  c.call("foo");
  ASSERT_EQUAL(4, the_one_default_arg);

  the_one_default_arg = 0;

  // define_module_function
  m.define_module_function("foo2", &method_with_one_default_arg, (Arg("num") = 4));

  m.call("foo2");
  ASSERT_EQUAL(4, the_one_default_arg);
}

TESTCASE(default_arguments_for_define_singleton_method)
{
  Class c(anonymous_class());
  c.define_singleton_method("foo", &defaults_method_one, (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true));

  c.call("foo", 2);

  ASSERT_EQUAL(2, defaults_method_one_arg1);
  ASSERT_EQUAL(3, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  c.call("foo", 11, 10);

  ASSERT_EQUAL(11, defaults_method_one_arg1);
  ASSERT_EQUAL(10, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  c.call("foo", 22, 33, false);

  ASSERT_EQUAL(22, defaults_method_one_arg1);
  ASSERT_EQUAL(33, defaults_method_one_arg2);
  ASSERT(!defaults_method_one_arg3);
}

TESTCASE(default_arguments_for_define_module_function)
{
  Module m(anonymous_module());
  m.define_module_function("foo", &defaults_method_one, (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true));

  m.call("foo", 2);

  ASSERT_EQUAL(2, defaults_method_one_arg1);
  ASSERT_EQUAL(3, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  m.call("foo", 11, 10);

  ASSERT_EQUAL(11, defaults_method_one_arg1);
  ASSERT_EQUAL(10, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  m.call("foo", 22, 33, false);

  ASSERT_EQUAL(22, defaults_method_one_arg1);
  ASSERT_EQUAL(33, defaults_method_one_arg2);
  ASSERT(!defaults_method_one_arg3);
}

namespace {
  std::string with_defaults_and_references_x;
  bool with_defaults_and_references_doIt;

  void with_defaults_and_references(std::string const& x, bool doIt = false)
  {
    with_defaults_and_references_x = x;
    with_defaults_and_references_doIt = doIt;
  }
}

TESTCASE(define_method_works_with_reference_arguments)
{
  Module m(anonymous_module());
  m.define_module_function("foo", &with_defaults_and_references,
      (Arg("x"), Arg("doIt") = false));

  m.call("foo", "test");

  ASSERT_EQUAL("test", with_defaults_and_references_x);
  ASSERT(!with_defaults_and_references_doIt);
}

namespace {
  class ReturnTest { };

  class Factory {
    public:
      Factory() { returnTest_ = new ReturnTest(); }

      const ReturnTest& getReturnTest() const {
        return *returnTest_;
      }

     private:
      const ReturnTest* returnTest_;
  };
}

TESTCASE(define_method_works_with_const_reference_return)
{
  define_class<ReturnTest>("ReturnTest")
    .define_constructor(Constructor<ReturnTest>());

  define_class<Factory>("Factory")
    .define_constructor(Constructor<Factory>())
    .define_method("get_return_test", &Factory::getReturnTest);

  Module m(anonymous_module());

  Object result = m.instance_eval("Factory.new.get_return_test");

  ASSERT_EQUAL("ReturnTest", result.class_of().name().c_str());
}

/*
namespace {
  float with_reference_defaults_x;
  std::string with_reference_defaults_str;

  void with_reference_defaults(float x, std::string const& str = std::string("testing"))
  {
    with_reference_defaults_x = x;
    with_reference_defaults_str = str;
  }
}

TESTCASE(define_method_works_with_reference_const_default_values)
{
  Module m(anonymous_module());
  m.define_module_function("bar", &with_reference_defaults,
      (Arg("x"), Arg("str") = std::string("testing")));

  m.call("bar", 3);

  ASSERT_EQUAL(3, with_reference_defaults_x);
  ASSERT_EQUAL("testing", with_reference_defaults_str);
}
*/
