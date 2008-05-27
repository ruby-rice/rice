#include "unittest.hpp"
#include "rice/Module.hpp"
#include "rice/Exception.hpp"
#include "rice/Array.hpp"

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

class Silly_Exception
  : public std::exception
{
};

void handle_silly_exception(Silly_Exception const & ex)
{
  throw Exception(rb_eRuntimeError, "SILLY");
}

void throw_silly_exception(Object self)
{
  throw Silly_Exception();
}

} // namespace

TESTCASE(add_handler)
{
  Module m(anonymous_module());
  m.add_handler<Silly_Exception>(handle_silly_exception);
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

TESTCASE(alias_method)
{
  Module m(anonymous_module());
  m.define_method("foo", define_method_simple_helper);
  m.alias_method("bar", "foo");
  define_method_simple_ok = false;
  Object o = m.instance_eval("o = Object.new; o.extend(self); o.bar");
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
  m.define_singleton_method("foo", define_method_int_foo_helper);
  define_method_int_result = 0;
  Foo * foo = new Foo;
  foo->x = 1024;
  VALUE f = Data_Wrap_Struct(rb_cObject, 0, Default_Allocation_Strategy<Foo>::free, foo);
  m.call("foo", 42, Object(f));
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

