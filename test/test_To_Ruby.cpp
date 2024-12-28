#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

#include <limits>
#include <cmath>

using namespace Rice;

TESTSUITE(ToRuby);

SETUP(ToRuby)
{
  embed_ruby();
}

TEARDOWN(ToRuby)
{
  rb_gc_start();
}

TESTCASE(object_to_ruby)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o.value(), detail::to_ruby(o));
}

TESTCASE(short_to_ruby)
{
  ASSERT_EQUAL(INT2NUM(0), detail::to_ruby((short)0));
  ASSERT_EQUAL(INT2NUM(-1), detail::to_ruby((short)-1));
  ASSERT_EQUAL(INT2NUM(1), detail::to_ruby((short)1));
  ASSERT_EQUAL(INT2NUM(std::numeric_limits<short>::min()),
               detail::to_ruby(std::numeric_limits<short>::min()));
  ASSERT_EQUAL(INT2NUM(std::numeric_limits<short>::max()),
               detail::to_ruby(std::numeric_limits<short>::max()));
}

TESTCASE(int_to_ruby)
{
  ASSERT(rb_equal(INT2NUM(0), detail::to_ruby((int)0)));
  ASSERT(rb_equal(INT2NUM(-1), detail::to_ruby((int)-1)));
  ASSERT(rb_equal(INT2NUM(1), detail::to_ruby((int)1)));
  ASSERT(rb_equal(INT2NUM(std::numeric_limits<int>::min()), detail::to_ruby(std::numeric_limits<int>::min())));
  ASSERT(rb_equal(INT2NUM(std::numeric_limits<int>::max()), detail::to_ruby(std::numeric_limits<int>::max())));
}

TESTCASE(long_to_ruby)
{
  ASSERT(rb_equal(LONG2NUM(0), detail::to_ruby((long)0)));
  ASSERT(rb_equal(LONG2NUM(-1), detail::to_ruby((long)-1)));
  ASSERT(rb_equal(LONG2NUM(1), detail::to_ruby((long)1)));
  ASSERT(rb_equal(LONG2NUM(FIXNUM_MAX), detail::to_ruby(FIXNUM_MAX)));
  ASSERT(rb_equal(LONG2NUM(FIXNUM_MIN), detail::to_ruby(FIXNUM_MIN)));
  ASSERT(rb_equal(LONG2NUM(std::numeric_limits<long>::min()), detail::to_ruby(std::numeric_limits<long>::min())));
  ASSERT(rb_equal(LONG2NUM(std::numeric_limits<long>::max()), detail::to_ruby(std::numeric_limits<long>::max())));
}

TESTCASE(long_long_to_ruby)
{
  ASSERT(rb_equal(LL2NUM(0), detail::to_ruby((long long)0)));
  ASSERT(rb_equal(LL2NUM(-1), detail::to_ruby((long long)-1)));
  ASSERT(rb_equal(LL2NUM(1), detail::to_ruby((long long)1)));
  ASSERT(rb_equal(LL2NUM(std::numeric_limits<long long>::min()), detail::to_ruby(std::numeric_limits<long long>::min())));
  ASSERT(rb_equal(LL2NUM(std::numeric_limits<long long>::max()), detail::to_ruby(std::numeric_limits<long long>::max())));
}

TESTCASE(unsigned_short_to_ruby)
{
  ASSERT(rb_equal(UINT2NUM(0), detail::to_ruby((unsigned short)0)));
  ASSERT(rb_equal(UINT2NUM(1), detail::to_ruby((unsigned short)1)));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned short>::min()), detail::to_ruby(std::numeric_limits<unsigned short>::min())));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned short>::max()), detail::to_ruby(std::numeric_limits<unsigned short>::max())));
}

TESTCASE(unsigned_int_to_ruby)
{
  ASSERT(rb_equal(UINT2NUM(0), detail::to_ruby((unsigned int)0)));
  ASSERT(rb_equal(UINT2NUM(1), detail::to_ruby((unsigned int)1)));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned int>::min()), detail::to_ruby(std::numeric_limits<unsigned int>::min())));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned int>::max()), detail::to_ruby(std::numeric_limits<unsigned int>::max())));
}

TESTCASE(unsigned_long_to_ruby)
{
  ASSERT(rb_equal(ULONG2NUM(0), detail::to_ruby((unsigned long)0)));
  ASSERT(rb_equal(ULONG2NUM(1), detail::to_ruby((unsigned long)1)));
  ASSERT(rb_equal(ULONG2NUM(FIXNUM_MAX), detail::to_ruby(FIXNUM_MAX)));
  ASSERT(rb_equal(ULONG2NUM(std::numeric_limits<unsigned long>::min()), detail::to_ruby(std::numeric_limits<unsigned long>::min())));
  ASSERT(rb_equal(ULONG2NUM(std::numeric_limits<unsigned long>::max()), detail::to_ruby(std::numeric_limits<unsigned long>::max())));
}

TESTCASE(unsigned_long_long_to_ruby)
{
  ASSERT(rb_equal(ULL2NUM(0), detail::to_ruby((unsigned long long)0)));
  ASSERT(rb_equal(ULL2NUM(1), detail::to_ruby((unsigned long long)1)));
  ASSERT(rb_equal(ULL2NUM(std::numeric_limits<unsigned long long>::min()), detail::to_ruby(std::numeric_limits<unsigned long long>::min())));
  ASSERT(rb_equal(ULL2NUM(std::numeric_limits<unsigned long long>::max()), detail::to_ruby(std::numeric_limits<unsigned long long>::max())));
}

TESTCASE(bool_to_ruby)
{
  ASSERT(rb_equal(Qfalse, detail::to_ruby(false)));
  ASSERT(rb_equal(Qtrue, detail::to_ruby(true)));
}

TESTCASE(float_to_ruby)
{
  ASSERT(rb_equal(rb_float_new(0.0f), detail::to_ruby(0.0f)));
  ASSERT(rb_equal(rb_float_new(-1.0f), detail::to_ruby(-1.0f)));
  ASSERT(rb_equal(rb_float_new(1.0f), detail::to_ruby(1.0f)));
  ASSERT(rb_equal(rb_float_new(0.5f), detail::to_ruby(0.5f)));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<float>::min()), detail::to_ruby(std::numeric_limits<float>::min())));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<float>::max()), detail::to_ruby(std::numeric_limits<float>::max())));
  ASSERT(Object(detail::to_ruby(std::numeric_limits<float>::quiet_NaN())).call("nan?"));
  ASSERT(Object(detail::to_ruby(std::numeric_limits<float>::signaling_NaN())).call("nan?"));
  ASSERT_EQUAL(rb_float_new(std::numeric_limits<float>::epsilon()),
               detail::to_ruby(std::numeric_limits<float>::epsilon()));
}

TESTCASE(double_to_ruby)
{
  ASSERT(rb_equal(rb_float_new(0.0f), detail::to_ruby(0.0f)));
  ASSERT(rb_equal(rb_float_new(-1.0f), detail::to_ruby(-1.0f)));
  ASSERT(rb_equal(rb_float_new(1.0f), detail::to_ruby(1.0f)));
  ASSERT(rb_equal(rb_float_new(0.5f), detail::to_ruby(0.5f)));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<double>::min()), detail::to_ruby(std::numeric_limits<double>::min())));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<double>::max()), detail::to_ruby(std::numeric_limits<double>::max())));
  ASSERT(Object(detail::to_ruby(std::numeric_limits<double>::quiet_NaN())).call("nan?"));
  ASSERT(Object(detail::to_ruby(std::numeric_limits<double>::signaling_NaN())).call("nan?"));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<double>::epsilon()), detail::to_ruby(std::numeric_limits<double>::epsilon())));
}

TESTCASE(char_ptr_to_ruby)
{
  detail::To_Ruby<char*> toRuby;
  ASSERT(rb_equal(String("").value(), toRuby.convert("")));
  ASSERT(rb_equal(String("A string").value(), toRuby.convert("A string")));
  ASSERT(rb_equal(Qnil, toRuby.convert(nullptr)));
}

TESTCASE(char_const_ptr_to_ruby)
{
  ASSERT(rb_equal(String("").value(), detail::to_ruby((char const *)"")));
  ASSERT(rb_equal(String("foo").value(), detail::to_ruby((char const *)"foo")));
  ASSERT(rb_equal(String("foo").value(), detail::to_ruby("foo")));
}

TESTCASE(char_const_array_to_ruby_symbol)
{
  ASSERT(rb_equal(Symbol("foo").value(), detail::to_ruby(":foo")));
}
