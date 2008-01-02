#include "unittest.hpp"
#include "rice/Exception.hpp"
#include "rice/String.hpp"

using namespace Rice;

TESTSUITE(Exception);

SETUP(Exception)
{
  ruby_init();
}

TESTCASE(construct_from_exception_object)
{
  VALUE v = protect(rb_exc_new2, rb_eRuntimeError, "foo");
  Exception ex(v);
  ASSERT_EQUAL(ex.value(), v);
}

TESTCASE(copy_construct)
{
  VALUE v = protect(rb_exc_new2, rb_eRuntimeError, "foo");
  Exception ex1(v);
  Exception ex2(v);
  ASSERT_EQUAL(ex2.value(), v);
}

TESTCASE(construct_from_format_string)
{
  Exception ex(rb_eRuntimeError, "%s", "foo");
  ASSERT_EQUAL(ex.class_of(), Object(rb_eRuntimeError));
}

TESTCASE(message)
{
  Exception ex(rb_eRuntimeError, "%s", "foo");
  ASSERT_EQUAL(String("foo"), ex.message());
}

TESTCASE(what)
{
  Exception ex(rb_eRuntimeError, "%s", "foo");
  ASSERT_EQUAL("foo", ex.what());
}

