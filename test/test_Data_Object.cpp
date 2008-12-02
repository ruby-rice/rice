#include "unittest.hpp"
#include "rice/Data_Object.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Exception.hpp"

using namespace Rice;

TESTSUITE(Data_Object);

namespace
{
  struct Foo { Foo() : x_(42) { } int x_; };

  bool test_ruby_mark__marked = false;
}

template<>
void ruby_mark(Foo * foo)
{
  test_ruby_mark__marked = true;
}

SETUP(Data_Object)
{
  ruby_init();

  if(!Data_Type<Foo>::is_bound())
  {
    Class object(rb_cObject);
    if(object.const_defined("Foo"))
    {
      object.remove_const("Foo");
    }

    define_class<Foo>("Foo");
  }
}

TESTCASE(construct_from_pointer_with_defaults)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo, wrapped_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), wrapped_foo.class_of());
  typedef void (*Mark_Func)(void *);
  typedef void (*Mark_Func_Foo)(Foo *);
  Mark_Func expected_mark_func =
    Mark_Func(Mark_Func_Foo(ruby_mark<Foo>));
  ASSERT_EQUAL(
      expected_mark_func,
      RDATA(wrapped_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(wrapped_foo.value()));
}

TESTCASE(construct_from_pointer_and_klass)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo, Data_Type<Foo>::klass());
  ASSERT_EQUAL(foo, wrapped_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), wrapped_foo.class_of());
  typedef void (*Mark_Func)(void *);
  typedef void (*Mark_Func_Foo)(Foo *);
  Mark_Func expected_mark_func =
    Mark_Func(Mark_Func_Foo(ruby_mark<Foo>));
  ASSERT_EQUAL(
      expected_mark_func,
      RDATA(wrapped_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(wrapped_foo.value()));
}

TESTCASE(construct_from_pointer_and_alternate_klass)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo, rb_cObject);
  ASSERT_EQUAL(foo, wrapped_foo.get());
  ASSERT_EQUAL(rb_cObject, CLASS_OF(wrapped_foo.value()));
  typedef void (*Mark_Func)(void *);
  typedef void (*Mark_Func_Foo)(Foo *);
  Mark_Func expected_mark_func =
    Mark_Func(Mark_Func_Foo(ruby_mark<Foo>));
  ASSERT_EQUAL(
      expected_mark_func,
      RDATA(wrapped_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(wrapped_foo.value()));
}

namespace
{
  void dummy_mark(Foo *)
  {
  }
}

TESTCASE(construct_from_pointer_and_klass_and_mark_function)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo, rb_cFoo, dummy_mark);
  ASSERT_EQUAL(foo, wrapped_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), wrapped_foo.class_of());
  ASSERT_EQUAL((void *)dummy_mark, RDATA(wrapped_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(wrapped_foo.value()));
}

namespace
{
  void my_free(Foo * f)
  {
    delete f;
  }
}

TESTCASE(construct_from_pointer_and_klass_and_mark_and_free)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo, rb_cFoo, dummy_mark, my_free);
  ASSERT_EQUAL(foo, wrapped_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), wrapped_foo.class_of());
  ASSERT_EQUAL((void *)dummy_mark, RDATA(wrapped_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(my_free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(wrapped_foo.value()));
}

TESTCASE(construct_from_ruby_object)
{
  Foo * foo = new Foo;
  VALUE wrapped_foo = Data_Wrap_Struct(Data_Type<Foo>::klass(), 0, Default_Allocation_Strategy<Foo>::free, foo);
  Data_Object<Foo> data_object_foo(wrapped_foo);
  ASSERT_EQUAL(foo, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RDATA(wrapped_foo), RDATA(data_object_foo.value()));
  ASSERT_EQUAL(RUBY_DATA_FUNC(0), RDATA(data_object_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(data_object_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(data_object_foo.value()));
}

TESTCASE(construct_from_ruby_object_and_class)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  VALUE wrapped_foo = Data_Wrap_Struct(Data_Type<Foo>::klass(), 0, Default_Allocation_Strategy<Foo>::free, foo);
  Data_Object<Foo> data_object_foo(wrapped_foo, rb_cFoo);
  ASSERT_EQUAL(foo, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RDATA(wrapped_foo), RDATA(data_object_foo.value()));
  ASSERT_EQUAL(RUBY_DATA_FUNC(0), RDATA(data_object_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(data_object_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(data_object_foo.value()));
}

TESTCASE(construct_from_ruby_object_and_wrong_class)
{
  Foo * foo = new Foo;
  Data_Type<Foo> rb_cFoo;
  VALUE wrapped_foo = Data_Wrap_Struct(rb_cObject, 0, Default_Allocation_Strategy<Foo>::free, foo);
  ASSERT_EXCEPTION_CHECK(
      Exception,
      Data_Object<Foo> data_object_foo(wrapped_foo, rb_cFoo),
      ASSERT_EQUAL(
          Object(rb_eTypeError),
          Object(CLASS_OF(ex.value()))
          )
      );
}

TESTCASE(copy_construct)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  VALUE wrapped_foo = Data_Wrap_Struct(Data_Type<Foo>::klass(), 0, Default_Allocation_Strategy<Foo>::free, foo);
  Data_Object<Foo> orig_data_object_foo(wrapped_foo, rb_cFoo);
  Data_Object<Foo> data_object_foo(orig_data_object_foo);
  ASSERT_EQUAL(foo, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RDATA(wrapped_foo), RDATA(data_object_foo.value()));
  ASSERT_EQUAL(RUBY_DATA_FUNC(0), RDATA(data_object_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Allocation_Strategy<Foo>::free),
      RUBY_DATA_FUNC(RDATA(data_object_foo.value())->dfree));
  ASSERT_EQUAL(foo, DATA_PTR(data_object_foo.value()));
}

TESTCASE(dereference)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo, &*wrapped_foo);
}

TESTCASE(arrow)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(42, foo->x_);
}

TESTCASE(get)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo, wrapped_foo.get());
}

// TODO: swap

TESTCASE(to_ruby)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(wrapped_foo.value(), to_ruby(wrapped_foo).value());
}

TESTCASE(from_ruby)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo, from_ruby<Foo *>(wrapped_foo));
}

TESTCASE(from_ruby_const_ref)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo->x_, from_ruby<Foo const &>(wrapped_foo).x_);
}

TESTCASE(from_ruby_copy)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo->x_, from_ruby<Foo>(wrapped_foo).x_);
}

TESTCASE(ruby_mark)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);

  test_ruby_mark__marked = false;

  rb_gc_start();

  ASSERT_EQUAL(true, test_ruby_mark__marked);
}

