#include "unittest.hpp"
#include "rice/Class.hpp"
#include "rice/Constructor.hpp"
#include "rice/protect.hpp"
#include "rice/Exception.hpp"
#include "rice/Array.hpp"
#include "rice/String.hpp"
#include "rice/Symbol.hpp"
#include <iostream>

using namespace Rice;
using namespace std;

TESTSUITE(Class);

SETUP(Class)
{
  ruby_init();
}

TESTCASE(construct)
{
  Class c(rb_cObject);
  ASSERT_EQUAL(rb_cObject, c.value());
}

TESTCASE(undef_creation_funcs)
{
  Class c(anonymous_class());
  Class & c2(c.undef_creation_funcs());
  ASSERT_EQUAL(&c, &c2);
  ASSERT_EXCEPTION_CHECK(
      Exception,
      c.call("new"),
      ASSERT_EQUAL(
          Object(rb_eTypeError), // TODO: 1.6.x?
          Object(CLASS_OF(ex.value()))
          )
      );
}

TESTCASE(include_module)
{
  Class c(anonymous_class());
  Class & c2(c.include_module(rb_mEnumerable));
  ASSERT_EQUAL(&c, &c2);
  Array ancestors(c.ancestors());
  Array expected_ancestors;
  expected_ancestors.push(c);
  expected_ancestors.push(Module(rb_mEnumerable));
  expected_ancestors.push(Module(rb_cObject));
  expected_ancestors.push(Module(rb_mKernel));
#ifdef RUBY_VM
  expected_ancestors.push(Module(rb_cBasicObject));
#endif
  ASSERT_EQUAL(expected_ancestors, ancestors);
}

TESTCASE(const_set_get_by_id)
{
  Class c(anonymous_class());
  Object v = to_ruby(42);
  Class & c2(c.const_set(rb_intern("FOO"), v));
  ASSERT_EQUAL(&c, &c2);
  ASSERT_EQUAL(v, c.const_get(rb_intern("FOO")));
}

TESTCASE(const_set_get_by_identifier)
{
  Class c(anonymous_class());
  Object v = to_ruby(42);
  Class & c2(c.const_set(Identifier("FOO"), v));
  ASSERT_EQUAL(&c, &c2);
  ASSERT_EQUAL(v, c.const_get(Identifier("FOO")));
}

TESTCASE(const_set_get_by_string)
{
  Class c(anonymous_class());
  Object v = to_ruby(42);
  Class & c2(c.const_set("FOO", v));
  ASSERT_EQUAL(&c, &c2);
  ASSERT_EQUAL(v, c.const_get("FOO"));
}

namespace
{

bool define_method_simple_ok;

void define_method_simple_helper()
{
  define_method_simple_ok = true;
}

} // namespace

TESTCASE(define_method_simple)
{
  Class c(anonymous_class());
  c.define_method("foo", &define_method_simple_helper);
  Object o = c.call("new");
  define_method_simple_ok = false;
  o.call("foo");
  ASSERT(define_method_simple_ok);
}

TESTCASE(define_singleton_method_simple)
{
  Class c(anonymous_class());
  c.define_singleton_method("foo", &define_method_simple_helper);
  define_method_simple_ok = false;
  Object o = c.call("foo");
  ASSERT(define_method_simple_ok);
}

TESTCASE(define_module_function_simple)
{
  // module_function only works with Module, not Class
  Class c(anonymous_class());
  ASSERT_EXCEPTION_CHECK(
      Exception,
      c.define_module_function("foo", &define_method_simple_helper),
      ASSERT_EQUAL(
          Object(rb_eTypeError),
          Object(CLASS_OF(ex.value()))
          )
      );
}

namespace
{
  class RefTest
  {
    public:
      RefTest() { }

      static std::string& getReference() {
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

namespace
{

  int define_method_int_result;

  class IntHelper {
    public:
      IntHelper() { }

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
      ASSERT_EQUAL(
          Object(rb_eArgError),
          Object(CLASS_OF(ex.value()))
          )
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

void define_method_int_foo_helper(int i, Foo * x)
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

TESTCASE(define_method_int_foo)
{
  Class c(anonymous_class());
  c.define_method("foo", &define_method_int_foo_helper);
  Object o = c.call("new");
  define_method_int_result = 0;
  Foo * foo = new Foo;
  foo->x = 1024;
  VALUE f = Data_Wrap_Struct(rb_cObject, 0, Default_Allocation_Strategy<Foo>::free, foo);
  o.call("foo", 42, Object(f));
  ASSERT_EQUAL(42, define_method_int_foo_result_i);
  ASSERT_EQUAL(foo, define_method_int_foo_result_x);
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

void throw_silly_exception()
{
  throw Silly_Exception();
}

}

TESTCASE(add_handler)
{
  Class c(rb_cObject);
  c.add_handler<Silly_Exception>(handle_silly_exception);
  c.define_method("foo", throw_silly_exception);
  Object exc = protect(rb_eval_string, "begin; foo; rescue Exception; $!; end");
  ASSERT_EQUAL(rb_eRuntimeError, CLASS_OF(exc));
  Exception ex(exc);
  ASSERT_EQUAL(String("SILLY"), String(ex.message()));
}

namespace
{

class Container
{
public:
  Container(int * array, size_t length)
    : array_(array)
    , length_(length)
  {
  }

  int * begin() { return array_; }
  int * end() { return array_ + length_; }

private:
  int * array_;
  size_t length_;
};

} // namespace

template<>
Container * from_ruby<Container *>(Object x)
{
  Container * retval;
  Data_Get_Struct(x.value(), Container, retval);
  return retval;
}

TESTCASE(define_iterator)
{
  int array[] = { 1, 2, 3 };
  Class c(anonymous_class());
  c.define_iterator(&Container::begin, &Container::end);
  Container * container = new Container(array, 3);
  Object wrapped_container = Data_Wrap_Struct(
      c, 0, Default_Allocation_Strategy<Container>::free, container);
  Array a = wrapped_container.instance_eval("a = []; each() { |x| a << x }; a");
  ASSERT_EQUAL(3u, a.size());
  ASSERT_EQUAL(to_ruby(1), Object(a[0]));
  ASSERT_EQUAL(to_ruby(2), Object(a[1]));
  ASSERT_EQUAL(to_ruby(3), Object(a[2]));
}

TESTCASE(define_class)
{
  Class object(rb_cObject);
  if(object.const_defined("Foo"))
  {
    object.remove_const("Foo");
  }

  Class c = define_class("Foo");

  ASSERT(c.is_a(rb_cClass));
  ASSERT_EQUAL(c, object.const_get("Foo"));
}

TESTCASE(define_class_under)
{
  Class object(rb_cObject);
  if(object.const_defined("Foo"))
  {
    object.remove_const("Foo");
  }

  Module math(rb_mMath);
  if(math.const_defined("Foo"))
  {
    math.remove_const("Foo");
  }

  Class c = define_class_under(math, "Foo");

  ASSERT(c.is_a(rb_cClass));
  ASSERT_EQUAL(c, math.const_get("Foo"));
  ASSERT(!object.const_defined("Foo"));
}

TESTCASE(module_define_class)
{
  Class object(rb_cObject);
  if(object.const_defined("Foo"))
  {
    object.remove_const("Foo");
  }

  Module math(rb_mMath);
  if(math.const_defined("Foo"))
  {
    math.remove_const("Foo");
  }

  Class c = math.define_class("Foo");

  ASSERT(c.is_a(rb_cClass));
  ASSERT_EQUAL(c, math.const_get("Foo"));
  ASSERT(!object.const_defined("Foo"));
}

namespace {
  class BaseClass {
    public:
      BaseClass() { }
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

namespace
{
  int defaults_method_one_arg1;
  int defaults_method_one_arg2;
  bool defaults_method_one_arg3 = false;

  class DefaultArgs
  {
    public:
      void defaults_method_one(int arg1, int arg2 = 3, bool arg3 = true)
      {
        defaults_method_one_arg1 = arg1;
        defaults_method_one_arg2 = arg2;
        defaults_method_one_arg3 = arg3;
      }
  };
}

TESTCASE(define_method_default_arguments)
{
  Class c = define_class<DefaultArgs>("DefaultArgs")
              .define_constructor(Constructor<DefaultArgs>())
              .define_method("with_defaults",
                  &DefaultArgs::defaults_method_one,
                  (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true));

  Object o = c.call("new");
  o.call("with_defaults", 2);

  ASSERT_EQUAL(2, defaults_method_one_arg1);
  ASSERT_EQUAL(3, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  o.call("with_defaults", 11, 10);

  ASSERT_EQUAL(11, defaults_method_one_arg1);
  ASSERT_EQUAL(10, defaults_method_one_arg2);
  ASSERT(defaults_method_one_arg3);

  o.call("with_defaults", 22, 33, false);

  ASSERT_EQUAL(22, defaults_method_one_arg1);
  ASSERT_EQUAL(33, defaults_method_one_arg2);
  ASSERT(!defaults_method_one_arg3);
}

/*
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
                  (Arg("x"), Arg("str") = std::string("testing")));

  Object o = c.call("new");
  o.call("bar", 3);

  ASSERT_EQUAL(3, with_reference_defaults_x);
  ASSERT_EQUAL("testing", with_reference_defaults_str);
}
*/
