#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

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
  embed_ruby();

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
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
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
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
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
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
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
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(wrapped_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
}

namespace
{
  void my_free(detail::Wrapper<Foo>* wrapper)
  {
    delete wrapper;
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
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
}

TESTCASE(construct_from_ruby_object)
{
  Foo * foo = new Foo;
  VALUE wrapped_foo = detail::wrap(Data_Type<Foo>::klass(), 
    (RUBY_DATA_FUNC)0, (RUBY_DATA_FUNC)Default_Free_Function<Foo>::free, foo);

  Data_Object<Foo> data_object_foo(wrapped_foo);
  ASSERT_EQUAL(foo, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RDATA(wrapped_foo), RDATA(data_object_foo.value()));
  ASSERT_EQUAL(RUBY_DATA_FUNC(0), RDATA(data_object_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(data_object_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
}

TESTCASE(construct_from_ruby_object_and_class)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  VALUE wrapped_foo = detail::wrap(Data_Type<Foo>::klass(), 
    (RUBY_DATA_FUNC)0, (RUBY_DATA_FUNC)Default_Free_Function<Foo>::free, foo);
  Data_Object<Foo> data_object_foo(wrapped_foo, rb_cFoo);
  ASSERT_EQUAL(foo, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RDATA(wrapped_foo), RDATA(data_object_foo.value()));
  ASSERT_EQUAL(RUBY_DATA_FUNC(0), RDATA(data_object_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(data_object_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
}

TESTCASE(construct_from_ruby_object_and_wrong_class)
{
  Foo * foo = new Foo;
  Data_Type<Foo> rb_cFoo;
  VALUE wrapped_foo = detail::wrap(rb_cObject, 
    nullptr, (RUBY_DATA_FUNC)Default_Free_Function<Foo>::free, foo);
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
  VALUE wrapped_foo = detail::wrap(Data_Type<Foo>::klass(), 
    (RUBY_DATA_FUNC)0, (RUBY_DATA_FUNC)Default_Free_Function<Foo>::free, foo);
  Data_Object<Foo> orig_data_object_foo(wrapped_foo, rb_cFoo);
  Data_Object<Foo> data_object_foo(orig_data_object_foo);
  ASSERT_EQUAL(foo, data_object_foo.get());
  ASSERT_EQUAL(Data_Type<Foo>::klass(), data_object_foo.class_of());
  ASSERT_EQUAL(RDATA(wrapped_foo), RDATA(data_object_foo.value()));
  ASSERT_EQUAL(RUBY_DATA_FUNC(0), RDATA(data_object_foo.value())->dmark);
  ASSERT_EQUAL(
      RUBY_DATA_FUNC(Default_Free_Function<Foo>::free),
      RUBY_DATA_FUNC(RDATA(data_object_foo.value())->dfree));
  ASSERT_EQUAL(foo, detail::unwrap<Foo>(wrapped_foo));
}

TESTCASE(move_construct)
{
  Data_Type<Foo> rb_cFoo;
  Foo* foo = new Foo;

  Data_Object<Foo> wrapper1(foo, rb_cFoo);
  Data_Object<Foo> wrapper2(std::move(wrapper1));

  ASSERT_EQUAL(foo, wrapper2.get());
  ASSERT((wrapper1.get() == nullptr));
}

TESTCASE(move_assign)
{
  Data_Type<Foo> rb_cFoo;

  Foo* foo1 = new Foo;
  Data_Object<Foo> wrapper1(foo1, rb_cFoo);

  Foo* foo2 = new Foo;
  Data_Object<Foo> wrapper2(foo2, rb_cFoo);

  wrapper2 = std::move(wrapper1);

  ASSERT_EQUAL(foo1, wrapper2.get());
  ASSERT((wrapper1.get() == nullptr));
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
  ASSERT_EQUAL(wrapped_foo.value(), detail::to_ruby(wrapped_foo));
}

TESTCASE(from_ruby)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo, detail::From_Ruby<Foo *>::convert(wrapped_foo));
}

TESTCASE(from_ruby_const_ref)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo->x_, detail::From_Ruby<Foo const &>::convert(wrapped_foo).x_);
}

TESTCASE(from_ruby_copy)
{
  Data_Type<Foo> rb_cFoo;
  Foo * foo = new Foo;
  Data_Object<Foo> wrapped_foo(foo);
  ASSERT_EQUAL(foo->x_, detail::From_Ruby<Foo>::convert(wrapped_foo).x_);
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
