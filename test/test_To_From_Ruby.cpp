#include "unittest.hpp"
#include "embed_ruby.hpp"
#include "rice.hpp"

#include <limits>
#include <cmath>

using namespace Rice;

TESTSUITE(To_From_Ruby);

SETUP(To_From_Ruby)
{
  embed_ruby();
}

TESTCASE(object_to_ruby)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o.value(), detail::to_ruby(o));
}

TESTCASE(object_from_ruby)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o, detail::From_Ruby<Object>::convert(o));
}

TESTCASE(short_to_ruby)
{
  ASSERT_EQUAL(INT2NUM(0), detail::to_ruby((short)0));
  ASSERT_EQUAL(INT2NUM(-1), detail::to_ruby((short)-1));
  ASSERT_EQUAL(INT2NUM(1), detail::to_ruby((short)1));
  ASSERT_EQUAL(
      INT2NUM(std::numeric_limits<short>::min()),
      detail::to_ruby(std::numeric_limits<short>::min()));
  ASSERT_EQUAL(
      INT2NUM(std::numeric_limits<short>::max()),
      detail::to_ruby(std::numeric_limits<short>::max()));
}

TESTCASE(short_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<short>::convert(INT2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<short>::convert(INT2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<short>::convert(INT2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<short>::min(),
      detail::From_Ruby<short>::convert(INT2NUM(std::numeric_limits<short>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<short>::max(),
      detail::From_Ruby<short>::convert(INT2NUM(std::numeric_limits<short>::max())));
}

TESTCASE(int_to_ruby)
{
  ASSERT(rb_equal(INT2NUM(0), detail::to_ruby((int)0)));
  ASSERT(rb_equal(INT2NUM(-1), detail::to_ruby((int)-1)));
  ASSERT(rb_equal(INT2NUM(1), detail::to_ruby((int)1)));
  ASSERT(rb_equal(INT2NUM(std::numeric_limits<int>::min()), detail::to_ruby(std::numeric_limits<int>::min())));
  ASSERT(rb_equal(INT2NUM(std::numeric_limits<int>::max()), detail::to_ruby(std::numeric_limits<int>::max())));
}

TESTCASE(int_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<int>::convert(INT2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<int>::convert(INT2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<int>::convert(INT2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<int>::min(),
      detail::From_Ruby<int>::convert(INT2NUM(std::numeric_limits<int>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<int>::max(),
      detail::From_Ruby<int>::convert(INT2NUM(std::numeric_limits<int>::max())));
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

TESTCASE(long_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<long>::convert(LONG2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<long>::convert(LONG2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<long>::convert(LONG2NUM(1)));
  ASSERT_EQUAL(
      FIXNUM_MIN,
      detail::From_Ruby<long>::convert(LONG2NUM(FIXNUM_MIN)));
  ASSERT_EQUAL(
      FIXNUM_MAX,
      detail::From_Ruby<long>::convert(LONG2NUM(FIXNUM_MAX)));
  ASSERT_EQUAL(
      std::numeric_limits<long>::min(),
      detail::From_Ruby<long>::convert(LONG2NUM(std::numeric_limits<long>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<long>::max(),
      detail::From_Ruby<long>::convert(LONG2NUM(std::numeric_limits<long>::max())));
}

TESTCASE(long_long_to_ruby)
{
  ASSERT(rb_equal(LL2NUM(0), detail::to_ruby((long long)0)));
  ASSERT(rb_equal(LL2NUM(-1), detail::to_ruby((long long)-1)));
  ASSERT(rb_equal(LL2NUM(1), detail::to_ruby((long long)1)));
  ASSERT(rb_equal(LL2NUM(std::numeric_limits<long long>::min()), detail::to_ruby(std::numeric_limits<long long>::min())));
  ASSERT(rb_equal(LL2NUM(std::numeric_limits<long long>::max()), detail::to_ruby(std::numeric_limits<long long>::max())));
}

TESTCASE(long_long_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<long long>::convert(LL2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<long long>::convert(LL2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<long long>::convert(LL2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<long long>::min(),
      detail::From_Ruby<long long>::convert(LL2NUM(std::numeric_limits<long long>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<long long>::max(),
      detail::From_Ruby<long long>::convert(LL2NUM(std::numeric_limits<long long>::max())));
}

TESTCASE(unsigned_short_to_ruby)
{
  ASSERT(rb_equal(UINT2NUM(0), detail::to_ruby((unsigned short)0)));
  ASSERT(rb_equal(UINT2NUM(1), detail::to_ruby((unsigned short)1)));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned short>::min()), detail::to_ruby(std::numeric_limits<unsigned short>::min())));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned short>::max()), detail::to_ruby(std::numeric_limits<unsigned short>::max())));
}

TESTCASE(unsigned_short_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned short>::convert(UINT2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned short>::convert(UINT2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned short>::min(),
      detail::From_Ruby<unsigned short>::convert(UINT2NUM(std::numeric_limits<unsigned short>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned short>::max(),
      detail::From_Ruby<unsigned short>::convert(UINT2NUM(std::numeric_limits<unsigned short>::max())));
}

TESTCASE(unsigned_int_to_ruby)
{
  ASSERT(rb_equal(UINT2NUM(0), detail::to_ruby((unsigned int)0)));
  ASSERT(rb_equal(UINT2NUM(1), detail::to_ruby((unsigned int)1)));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned int>::min()), detail::to_ruby(std::numeric_limits<unsigned int>::min())));
  ASSERT(rb_equal(UINT2NUM(std::numeric_limits<unsigned int>::max()), detail::to_ruby(std::numeric_limits<unsigned int>::max())));
}

TESTCASE(unsigned_int_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned int>::convert(UINT2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned int>::convert(UINT2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned int>::min(),
      detail::From_Ruby<unsigned int>::convert(UINT2NUM(std::numeric_limits<unsigned int>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned int>::max(),
      detail::From_Ruby<unsigned int>::convert(UINT2NUM(std::numeric_limits<unsigned int>::max())));
}

TESTCASE(unsigned_long_to_ruby)
{
  ASSERT(rb_equal(ULONG2NUM(0), detail::to_ruby((unsigned long)0)));
  ASSERT(rb_equal(ULONG2NUM(1), detail::to_ruby((unsigned long)1)));
  ASSERT(rb_equal(ULONG2NUM(FIXNUM_MAX), detail::to_ruby(FIXNUM_MAX)));
  ASSERT(rb_equal(ULONG2NUM(std::numeric_limits<unsigned long>::min()), detail::to_ruby(std::numeric_limits<unsigned long>::min())));
  ASSERT(rb_equal(ULONG2NUM(std::numeric_limits<unsigned long>::max()), detail::to_ruby(std::numeric_limits<unsigned long>::max())));
}

TESTCASE(unsigned_long_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned long>::convert(ULONG2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned long>::convert(ULONG2NUM(1)));
  ASSERT_EQUAL(
      static_cast<unsigned long>(FIXNUM_MIN),
      detail::From_Ruby<unsigned long>::convert(ULONG2NUM(FIXNUM_MIN)));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned long>::min(),
      detail::From_Ruby<unsigned long>::convert(ULONG2NUM(std::numeric_limits<unsigned long>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned long>::max(),
      detail::From_Ruby<unsigned long>::convert(ULONG2NUM(std::numeric_limits<unsigned long>::max())));
}

TESTCASE(unsigned_long_long_to_ruby)
{
  ASSERT(rb_equal(ULL2NUM(0), detail::to_ruby((unsigned long long)0)));
  ASSERT(rb_equal(ULL2NUM(1), detail::to_ruby((unsigned long long)1)));
  ASSERT(rb_equal(ULL2NUM(std::numeric_limits<unsigned long long>::min()), detail::to_ruby(std::numeric_limits<unsigned long long>::min())));
  ASSERT(rb_equal(ULL2NUM(std::numeric_limits<unsigned long long>::max()), detail::to_ruby(std::numeric_limits<unsigned long long>::max())));
}

TESTCASE(unsigned_long_long_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned long>::convert(ULL2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned long>::convert(ULL2NUM(1)));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned long long>::min(),
      detail::From_Ruby<unsigned long long>::convert(ULL2NUM(std::numeric_limits<unsigned long long>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<unsigned long long>::max(),
      detail::From_Ruby<unsigned long long>::convert(ULL2NUM(std::numeric_limits<unsigned long long>::max())));
}

TESTCASE(bool_to_ruby)
{
  ASSERT(rb_equal(Qfalse, detail::to_ruby(false)));
  ASSERT(rb_equal(Qtrue, detail::to_ruby(true)));
}

TESTCASE(bool_from_ruby)
{
  ASSERT_EQUAL(false, detail::From_Ruby<bool>::convert(Object(Qfalse)));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>::convert(Object(Qtrue)));
}

TESTCASE(float_to_ruby)
{
  ASSERT(rb_equal(rb_float_new(0.0f), detail::to_ruby(0.0f)));
  ASSERT(rb_equal(rb_float_new(-1.0f), detail::to_ruby(-1.0f)));
  ASSERT(rb_equal(rb_float_new(1.0f), detail::to_ruby(1.0f)));
  ASSERT(rb_equal(rb_float_new(0.5f), detail::to_ruby(0.5f)));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<float>::min()), detail::to_ruby(std::numeric_limits<float>::min())));
  ASSERT(rb_equal(rb_float_new(std::numeric_limits<float>::max()), detail::to_ruby(std::numeric_limits<float>::max())));
  ASSERT(
      Object(detail::to_ruby(std::numeric_limits<float>::quiet_NaN())).call("nan?"));
  ASSERT(
      Object(detail::to_ruby(std::numeric_limits<float>::signaling_NaN())).call("nan?"));
  ASSERT_EQUAL(
      rb_float_new(std::numeric_limits<float>::epsilon()),
      detail::to_ruby(std::numeric_limits<float>::epsilon()));
}

TESTCASE(float_from_ruby)
{
  ASSERT_EQUAL(0.0f, detail::From_Ruby<float>::convert(rb_float_new(0.0f)));
  ASSERT_EQUAL(-1.0f, detail::From_Ruby<float>::convert(rb_float_new(-1.0f)));
  ASSERT_EQUAL(1.0f, detail::From_Ruby<float>::convert(rb_float_new(1.0f)));
  ASSERT_EQUAL(
      std::numeric_limits<float>::min(),
      detail::From_Ruby<float>::convert(rb_float_new(std::numeric_limits<float>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<float>::max(),
      detail::From_Ruby<float>::convert(rb_float_new(std::numeric_limits<float>::max())));
  ASSERT(
      std::isnan(detail::From_Ruby<float>::convert(rb_float_new(std::numeric_limits<float>::quiet_NaN()))));
  ASSERT(
      std::isnan(detail::From_Ruby<float>::convert(rb_float_new(std::numeric_limits<float>::signaling_NaN()))));
  ASSERT_EQUAL(
      std::numeric_limits<float>::epsilon(),
      detail::From_Ruby<float>::convert(rb_float_new(std::numeric_limits<float>::epsilon())));
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

TESTCASE(double_from_ruby)
{
  ASSERT_EQUAL(0.0f, detail::From_Ruby<double>::convert(rb_float_new(0.0f)));
  ASSERT_EQUAL(-1.0f, detail::From_Ruby<double>::convert(rb_float_new(-1.0f)));
  ASSERT_EQUAL(1.0f, detail::From_Ruby<double>::convert(rb_float_new(1.0f)));
  ASSERT_EQUAL(
      std::numeric_limits<double>::min(),
      detail::From_Ruby<double>::convert(rb_float_new(std::numeric_limits<double>::min())));
  ASSERT_EQUAL(
      std::numeric_limits<double>::max(),
      detail::From_Ruby<double>::convert(rb_float_new(std::numeric_limits<double>::max())));
  ASSERT(
      std::isnan(detail::From_Ruby<double>::convert(rb_float_new(std::numeric_limits<double>::quiet_NaN()))));
  ASSERT(
      std::isnan(detail::From_Ruby<double>::convert(rb_float_new(std::numeric_limits<double>::signaling_NaN()))));
  ASSERT_EQUAL(
      std::numeric_limits<double>::epsilon(),
      detail::From_Ruby<double>::convert(rb_float_new(std::numeric_limits<double>::epsilon())));
}

TESTCASE(char_const_ptr_to_ruby)
{
  ASSERT(rb_equal(String("").value(), detail::to_ruby((char const *)"")));
  ASSERT(rb_equal(String("foo").value(), detail::to_ruby((char const *)"foo")));
}

TESTCASE(char_const_ptr_from_ruby)
{
  char const* foo = "foo";
  ASSERT_EQUAL("", detail::From_Ruby<char const *>::convert(rb_str_new2("")));
  ASSERT_EQUAL(foo, detail::From_Ruby<char const *>::convert(rb_str_new2("foo")));
}

TESTCASE(char_from_ruby_single_character_string)
{
  ASSERT_EQUAL('x', detail::From_Ruby<char>::convert(rb_str_new2("x")));
}

TESTCASE(char_from_ruby_longer_string)
{
  ASSERT_EXCEPTION(
    std::invalid_argument,
    detail::From_Ruby<char>::convert(rb_str_new2("xy"))
  );
}

TESTCASE(char_from_ruby_fixnum)
{
  ASSERT_EQUAL('1', detail::From_Ruby<char>::convert(INT2NUM(49)));
}

TESTCASE(char_star_from_ruby)
{
  const char* expected = "my string";
  ASSERT_EQUAL(expected, detail::From_Ruby<const char*>::convert(rb_str_new2("my string")));
}

TESTCASE(std_string_to_ruby)
{
  ASSERT(rb_equal(String("").value(), detail::to_ruby(std::string(""))));
  ASSERT(rb_equal(String("foo").value(), detail::to_ruby(std::string("foo"))));
}

TESTCASE(std_string_from_ruby)
{
  ASSERT_EQUAL(std::string(""), detail::From_Ruby<std::string>::convert(rb_str_new2("")));
  ASSERT_EQUAL(std::string("foo"), detail::From_Ruby<std::string>::convert(rb_str_new2("foo")));
}

TESTCASE(std_string_to_ruby_with_binary)
{
  Rice::String got = detail::to_ruby(std::string("\000test", 5));

  ASSERT_EQUAL(String(std::string("\000test", 5)), got);
  ASSERT_EQUAL(5ul, got.length());
}

TESTCASE(std_string_from_ruby_with_binary)
{
  std::string got = detail::From_Ruby<std::string>::convert(rb_str_new("\000test", 5));
  ASSERT_EQUAL(5ul, got.length());
  ASSERT_EQUAL(std::string("\000test", 5), got);
}

TESTCASE(array_to_ruby)
{
  Array a(rb_ary_new());
  ASSERT(rb_equal(a.value(), detail::to_ruby(a)));
}

TESTCASE(array_ref_to_ruby)
{
  Array a(rb_ary_new());
  Array& ref = a;
  ASSERT(rb_equal(a.value(), detail::to_ruby(ref)));
}

TESTCASE(array_ptr_to_ruby)
{
  Array a(rb_ary_new());
  Array* ptr = &a;
  ASSERT(rb_equal(a.value(), detail::to_ruby(ptr)));
}

TESTCASE(array_from_ruby)
{
  Array a(rb_ary_new());
  ASSERT_EQUAL(a, detail::From_Ruby<Array>::convert(a));
}

TESTCASE(hash_to_ruby)
{
  Hash h(rb_hash_new());
  ASSERT(rb_equal(h.value(), detail::to_ruby(h)));
}

TESTCASE(hash_from_ruby)
{
  Hash h(rb_hash_new());
  ASSERT_EQUAL(h, detail::From_Ruby<Hash>::convert(h));
}
