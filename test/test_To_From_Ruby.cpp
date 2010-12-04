#include "unittest.hpp"
#include "rice/to_from_ruby.hpp"
#include "rice/String.hpp"
#include <limits>
#include <cmath>

using namespace Rice;

TESTSUITE(To_From_Ruby);

SETUP(To_From_Ruby)
{
  ruby_init();
}

TESTCASE(object_to_ruby)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o.value(), to_ruby(o).value());
}

TESTCASE(object_from_ruby)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o.value(), from_ruby<Object>(o).value());
}

TESTCASE(int_to_ruby)
{
  ASSERT_EQUAL(INT2NUM(0), to_ruby((int)0).value());
  ASSERT_EQUAL(INT2NUM(-1), to_ruby((int)-1).value());
  ASSERT_EQUAL(INT2NUM(1), to_ruby((int)1).value());
  ASSERT_EQUAL(
      Object(INT2NUM(std::numeric_limits<int>::min())),
      to_ruby(std::numeric_limits<int>::min()));
  ASSERT_EQUAL(
      Object(INT2NUM(std::numeric_limits<int>::max())),
      to_ruby(std::numeric_limits<int>::max()));
}

TESTCASE(int_from_ruby)
{
  ASSERT_EQUAL(0, from_ruby<int>(INT2NUM(0)));
  ASSERT_EQUAL(-1, from_ruby<int>(INT2NUM(-1)));
  ASSERT_EQUAL(1, from_ruby<int>(INT2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<int>::min(),
      from_ruby<int>(INT2NUM(std::numeric_limits<int>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<int>::max(),
      from_ruby<int>(INT2NUM(std::numeric_limits<int>::max())));
}

TESTCASE(long_to_ruby)
{
  ASSERT_EQUAL(LONG2NUM(0), to_ruby((long)0).value());
  ASSERT_EQUAL(LONG2NUM(-1), to_ruby((long)-1).value());
  ASSERT_EQUAL(LONG2NUM(1), to_ruby((long)1).value());
  ASSERT_EQUAL(
      LONG2NUM(FIXNUM_MAX),
      to_ruby(FIXNUM_MAX).value());
  ASSERT_EQUAL(
      LONG2NUM(FIXNUM_MIN),
      to_ruby(FIXNUM_MIN).value());
  ASSERT_EQUAL(
      Object(LONG2NUM(std::numeric_limits<long>::min())),
      to_ruby(std::numeric_limits<long>::min()));
  ASSERT_EQUAL(
      Object(LONG2NUM(std::numeric_limits<long>::max())),
      to_ruby(std::numeric_limits<long>::max()));
}

TESTCASE(long_from_ruby)
{
  ASSERT_EQUAL(0, from_ruby<long>(LONG2NUM(0)));
  ASSERT_EQUAL(-1, from_ruby<long>(LONG2NUM(-1)));
  ASSERT_EQUAL(1, from_ruby<long>(LONG2NUM(1)));
  ASSERT_EQUAL(
      FIXNUM_MIN,
      from_ruby<long>(LONG2NUM(FIXNUM_MIN)));
  ASSERT_EQUAL(
      FIXNUM_MAX,
      from_ruby<long>(LONG2NUM(FIXNUM_MAX)));
  ASSERT_EQUAL(
      std::numeric_limits<long>::min(),
      from_ruby<long>(LONG2NUM(std::numeric_limits<long>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<long>::max(),
      from_ruby<long>(LONG2NUM(std::numeric_limits<long>::max())));
}

TESTCASE(unsigned_int_to_ruby)
{
  ASSERT_EQUAL(UINT2NUM(0), to_ruby((unsigned int)0).value());
  ASSERT_EQUAL(UINT2NUM(1), to_ruby((unsigned int)1).value());
  ASSERT_EQUAL(
      Object(UINT2NUM(std::numeric_limits<unsigned int>::min())),
      to_ruby(std::numeric_limits<unsigned int>::min()));
  ASSERT_EQUAL(
      Object(UINT2NUM(std::numeric_limits<unsigned int>::max())),
      to_ruby(std::numeric_limits<unsigned int>::max()));
}

TESTCASE(unsigned_int_from_ruby)
{
  ASSERT_EQUAL(0u, from_ruby<unsigned int>(UINT2NUM(0)));
  ASSERT_EQUAL(1u, from_ruby<unsigned int>(UINT2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned int>::min(),
      from_ruby<unsigned int>(UINT2NUM(std::numeric_limits<unsigned int>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned int>::max(),
      from_ruby<unsigned int>(UINT2NUM(std::numeric_limits<unsigned int>::max())));
}

TESTCASE(unsigned_long_to_ruby)
{
  ASSERT_EQUAL(ULONG2NUM(0), to_ruby((unsigned long)0).value());
  ASSERT_EQUAL(ULONG2NUM(1), to_ruby((unsigned long)1).value());
  ASSERT_EQUAL(
      ULONG2NUM(FIXNUM_MAX),
      to_ruby(FIXNUM_MAX).value());
  ASSERT_EQUAL(
      Object(ULONG2NUM(std::numeric_limits<unsigned long>::min())),
      to_ruby(std::numeric_limits<unsigned long>::min()));
  ASSERT_EQUAL(
      Object(ULONG2NUM(std::numeric_limits<unsigned long>::max())),
      to_ruby(std::numeric_limits<unsigned long>::max()));
}

TESTCASE(unsigned_long_from_ruby)
{
  ASSERT_EQUAL(0u, from_ruby<unsigned long>(ULONG2NUM(0)));
  ASSERT_EQUAL(1u, from_ruby<unsigned long>(ULONG2NUM(1)));
  ASSERT_EQUAL(
      static_cast<unsigned long>(FIXNUM_MIN),
      from_ruby<unsigned long>(ULONG2NUM(FIXNUM_MIN)));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned long>::min(),
      from_ruby<unsigned long>(ULONG2NUM(std::numeric_limits<unsigned long>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned long>::max(),
      from_ruby<unsigned long>(ULONG2NUM(std::numeric_limits<unsigned long>::max())));
}

TESTCASE(bool_to_ruby)
{
  ASSERT_EQUAL(Qfalse, to_ruby(false).value());
  ASSERT_EQUAL(Qtrue, to_ruby(true).value());
}

TESTCASE(bool_from_ruby)
{
  ASSERT_EQUAL(false, from_ruby<bool>(Object(Qfalse)));
  ASSERT_EQUAL(true, from_ruby<bool>(Object(Qtrue)));
}

TESTCASE(float_to_ruby)
{
  ASSERT_EQUAL(Object(rb_float_new(0.0f)), to_ruby(0.0f));
  ASSERT_EQUAL(Object(rb_float_new(-1.0f)), to_ruby(-1.0f));
  ASSERT_EQUAL(Object(rb_float_new(1.0f)), to_ruby(1.0f));
  ASSERT_EQUAL(Object(rb_float_new(0.5f)), to_ruby(0.5f));
  ASSERT_EQUAL(
      Object(rb_float_new(std::numeric_limits<float>::min())),
      to_ruby(std::numeric_limits<float>::min()));
  ASSERT_EQUAL(
      Object(rb_float_new(std::numeric_limits<float>::max())),
      to_ruby(std::numeric_limits<float>::max()));
  ASSERT(
      to_ruby(std::numeric_limits<float>::quiet_NaN()).call("nan?"));
  ASSERT(
      to_ruby(std::numeric_limits<float>::signaling_NaN()).call("nan?"));
  ASSERT_EQUAL(
      Object(rb_float_new(std::numeric_limits<float>::epsilon())),
      to_ruby(std::numeric_limits<float>::epsilon()));
}

TESTCASE(float_from_ruby)
{
  ASSERT_EQUAL(0.0f, from_ruby<float>(rb_float_new(0.0f)));
  ASSERT_EQUAL(-1.0f, from_ruby<float>(rb_float_new(-1.0f)));
  ASSERT_EQUAL(1.0f, from_ruby<float>(rb_float_new(1.0f)));
  ASSERT_EQUAL(
      std::numeric_limits<float>::min(),
      from_ruby<float>(rb_float_new(std::numeric_limits<float>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<float>::max(),
      from_ruby<float>(rb_float_new(std::numeric_limits<float>::max())));
  ASSERT(
      std::isnan(from_ruby<float>(rb_float_new(std::numeric_limits<float>::quiet_NaN()))));
  ASSERT(
      std::isnan(from_ruby<float>(rb_float_new(std::numeric_limits<float>::signaling_NaN()))));
  ASSERT_EQUAL(
      std::numeric_limits<float>::epsilon(),
      from_ruby<float>(rb_float_new(std::numeric_limits<float>::epsilon())));
}

TESTCASE(double_to_ruby)
{
  ASSERT_EQUAL(Object(rb_float_new(0.0f)), to_ruby(0.0f));
  ASSERT_EQUAL(Object(rb_float_new(-1.0f)), to_ruby(-1.0f));
  ASSERT_EQUAL(Object(rb_float_new(1.0f)), to_ruby(1.0f));
  ASSERT_EQUAL(Object(rb_float_new(0.5f)), to_ruby(0.5f));
  ASSERT_EQUAL(
      Object(rb_float_new(std::numeric_limits<double>::min())),
      to_ruby(std::numeric_limits<double>::min()));
  ASSERT_EQUAL(
      Object(rb_float_new(std::numeric_limits<double>::max())),
      to_ruby(std::numeric_limits<double>::max()));
  ASSERT(
      to_ruby(std::numeric_limits<double>::quiet_NaN()).call("nan?"));
  ASSERT(
      to_ruby(std::numeric_limits<double>::signaling_NaN()).call("nan?"));
  ASSERT_EQUAL(
      Object(rb_float_new(std::numeric_limits<double>::epsilon())),
      to_ruby(std::numeric_limits<double>::epsilon()));
}

TESTCASE(double_from_ruby)
{
  ASSERT_EQUAL(0.0f, from_ruby<double>(rb_float_new(0.0f)));
  ASSERT_EQUAL(-1.0f, from_ruby<double>(rb_float_new(-1.0f)));
  ASSERT_EQUAL(1.0f, from_ruby<double>(rb_float_new(1.0f)));
  ASSERT_EQUAL(
      std::numeric_limits<double>::min(),
      from_ruby<double>(rb_float_new(std::numeric_limits<double>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<double>::max(),
      from_ruby<double>(rb_float_new(std::numeric_limits<double>::max())));
  ASSERT(
      std::isnan(from_ruby<double>(rb_float_new(std::numeric_limits<double>::quiet_NaN()))));
  ASSERT(
      std::isnan(from_ruby<double>(rb_float_new(std::numeric_limits<double>::signaling_NaN()))));
  ASSERT_EQUAL(
      std::numeric_limits<double>::epsilon(),
      from_ruby<double>(rb_float_new(std::numeric_limits<double>::epsilon())));
}

TESTCASE(char_const_ptr_to_ruby)
{
  ASSERT_EQUAL(String(""), to_ruby((char const *)""));
  ASSERT_EQUAL(String("foo"), to_ruby((char const *)"foo"));
}

TESTCASE(char_const_ptr_from_ruby)
{
  ASSERT_EQUAL("", from_ruby<char const *>(rb_str_new2("")));
  ASSERT_EQUAL("foo", from_ruby<char const *>(rb_str_new2("foo")));
}

TESTCASE(std_string_to_ruby)
{
  ASSERT_EQUAL(String(""), to_ruby(std::string("")));
  ASSERT_EQUAL(String("foo"), to_ruby(std::string("foo")));
}

TESTCASE(std_string_from_ruby)
{
  ASSERT_EQUAL(std::string(""), from_ruby<std::string>(rb_str_new2("")));
  ASSERT_EQUAL(std::string("foo"), from_ruby<std::string>(rb_str_new2("foo")));
}

TESTCASE(std_string_to_ruby_with_binary)
{
  Rice::String got = to_ruby(std::string("\000test", 5));

  ASSERT_EQUAL(String(std::string("\000test", 5)), got);
  ASSERT_EQUAL(5, got.length());
}

TESTCASE(std_string_from_ruby_with_binary)
{
  std::string got = from_ruby<std::string>(rb_str_new("\000test", 5));
  ASSERT_EQUAL(5, got.length());
  ASSERT_EQUAL(std::string("\000test", 5), got);
}
