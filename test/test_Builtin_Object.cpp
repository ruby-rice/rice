#include "unittest.hpp"
#include "rice/Builtin_Object.hpp"
#include "rice/Class.hpp"

using namespace Rice;

TESTSUITE(Builtin_Object);

SETUP(Builtin_Object)
{
  ruby_init();
}

TESTCASE(construct_with_object)
{
  Class c(rb_cObject);
  Object o(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b(o);
  ASSERT_EQUAL(o.value(), b.value());
  ASSERT_EQUAL(T_OBJECT, rb_type(b.value()));
  ASSERT_EQUAL(rb_cObject, b.class_of().value());
  ASSERT_EQUAL(rb_cObject, CLASS_OF(b.value()));
}

TESTCASE(copy_construct)
{
  Class c(rb_cObject);
  Object o(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b(o);
  Builtin_Object<RObject, T_OBJECT> b2(b);
  ASSERT_EQUAL(o.value(), b2.value());
  ASSERT_EQUAL(T_OBJECT, rb_type(b2.value()));
  ASSERT_EQUAL(rb_cObject, b2.class_of().value());
  ASSERT_EQUAL(rb_cObject, CLASS_OF(b2.value()));
}

TESTCASE(dereference)
{
  Class c(rb_cObject);
  Object o(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b(o);
  ASSERT_EQUAL(ROBJECT(o.value()), &*b);
}

TESTCASE(arrow)
{
  Class c(rb_cObject);
  Object o(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b(o);
  ASSERT_EQUAL(rb_cObject, b->basic.klass);
}

TESTCASE(get)
{
  Class c(rb_cObject);
  Object o(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b(o);
  ASSERT_EQUAL(ROBJECT(o.value()), b.get());
}

TESTCASE(swap)
{
  Class c(rb_cObject);
  Object o1(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b1(o1);
  Object o2(c.call("new"));
  Builtin_Object<RObject, T_OBJECT> b2(o2);
  b1.swap(b2);
  ASSERT_EQUAL(b1.value(), o2.value());
  ASSERT_EQUAL(b2.value(), o1.value());
}

