#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <limits>
#include <cmath>

using namespace Rice;

TESTSUITE(FromRuby);

SETUP(FromRuby)
{
  embed_ruby();
}

TEARDOWN(FromRuby)
{
  rb_gc_start();
}

/*TESTCASE(object_from_ruby)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o, detail::From_Ruby<Object>().convert(o));
}

TESTCASE(short_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<short>().convert(INT2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<short>().convert(INT2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<short>().convert(INT2NUM(1)));
  ASSERT_EQUAL(std::numeric_limits<short>::min(),
               detail::From_Ruby<short>().convert(INT2NUM(std::numeric_limits<short>::min())));
  ASSERT_EQUAL(std::numeric_limits<short>::max(),
               detail::From_Ruby<short>().convert(INT2NUM(std::numeric_limits<short>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<short>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

TESTCASE(int_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<int>().convert(INT2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<int>().convert(INT2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(INT2NUM(1)));
  ASSERT_EQUAL(std::numeric_limits<int>::min(),
               detail::From_Ruby<int>().convert(INT2NUM(std::numeric_limits<int>::min())));
  ASSERT_EQUAL(std::numeric_limits<int>::max(),
               detail::From_Ruby<int>().convert(INT2NUM(std::numeric_limits<int>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<int>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

TESTCASE(long_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<long>().convert(LONG2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<long>().convert(LONG2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<long>().convert(LONG2NUM(1)));
  ASSERT_EQUAL(FIXNUM_MIN, detail::From_Ruby<long>().convert(LONG2NUM(FIXNUM_MIN)));
  ASSERT_EQUAL(FIXNUM_MAX, detail::From_Ruby<long>().convert(LONG2NUM(FIXNUM_MAX)));
  ASSERT_EQUAL(std::numeric_limits<long>::min(), detail::From_Ruby<long>().convert(LONG2NUM(std::numeric_limits<long>::min())));
  ASSERT_EQUAL(std::numeric_limits<long>::max(), detail::From_Ruby<long>().convert(LONG2NUM(std::numeric_limits<long>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<long>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

TESTCASE(long_long_from_ruby)
{
  ASSERT_EQUAL(0, detail::From_Ruby<long long>().convert(LL2NUM(0)));
  ASSERT_EQUAL(-1, detail::From_Ruby<long long>().convert(LL2NUM(-1)));
  ASSERT_EQUAL(1, detail::From_Ruby<long long>().convert(LL2NUM(1)));
  ASSERT_EQUAL(std::numeric_limits<long long>::min(),
               detail::From_Ruby<long long>().convert(LL2NUM(std::numeric_limits<long long>::min())));
  ASSERT_EQUAL(std::numeric_limits<long long>::max(),
               detail::From_Ruby<long long>().convert(LL2NUM(std::numeric_limits<long long>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<long long>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion from string", ex.what())
  );
}

TESTCASE(unsigned_short_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned short>().convert(UINT2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned short>().convert(UINT2NUM(1)));
  ASSERT_EQUAL(std::numeric_limits<unsigned short>::min(),
               detail::From_Ruby<unsigned short>().convert(UINT2NUM(std::numeric_limits<unsigned short>::min())));
  ASSERT_EQUAL(std::numeric_limits<unsigned short>::max(),
               detail::From_Ruby<unsigned short>().convert(UINT2NUM(std::numeric_limits<unsigned short>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<unsigned short>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

TESTCASE(unsigned_int_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned int>().convert(UINT2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned int>().convert(UINT2NUM(1)));
  ASSERT_EQUAL(std::numeric_limits<unsigned int>::min(),
               detail::From_Ruby<unsigned int>().convert(UINT2NUM(std::numeric_limits<unsigned int>::min())));
  ASSERT_EQUAL(std::numeric_limits<unsigned int>::max(),
               detail::From_Ruby<unsigned int>().convert(UINT2NUM(std::numeric_limits<unsigned int>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<unsigned int>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

TESTCASE(unsigned_long_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned long>().convert(ULONG2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned long>().convert(ULONG2NUM(1)));
  ASSERT_EQUAL(static_cast<unsigned long>(FIXNUM_MIN),
               detail::From_Ruby<unsigned long>().convert(ULONG2NUM(FIXNUM_MIN)));
  ASSERT_EQUAL(std::numeric_limits<unsigned long>::min(),
               detail::From_Ruby<unsigned long>().convert(ULONG2NUM(std::numeric_limits<unsigned long>::min())));
  ASSERT_EQUAL(std::numeric_limits<unsigned long>::max(),
               detail::From_Ruby<unsigned long>().convert(ULONG2NUM(std::numeric_limits<unsigned long>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<unsigned long>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );
}

TESTCASE(unsigned_long_long_from_ruby)
{
  ASSERT_EQUAL(0u, detail::From_Ruby<unsigned long>().convert(ULL2NUM(0)));
  ASSERT_EQUAL(1u, detail::From_Ruby<unsigned long>().convert(ULL2NUM(1)));
  ASSERT_EQUAL(std::numeric_limits<unsigned long long>::min(),
               detail::From_Ruby<unsigned long long>().convert(ULL2NUM(std::numeric_limits<unsigned long long>::min())));
  ASSERT_EQUAL(std::numeric_limits<unsigned long long>::max(),
               detail::From_Ruby<unsigned long long>().convert(ULL2NUM(std::numeric_limits<unsigned long long>::max())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<unsigned long long>().convert(rb_str_new2("bad value")),
    ASSERT(std::string(ex.what()).find("no implicit conversion") == 0)
  );
}

TESTCASE(bool_from_ruby)
{
  ASSERT_EQUAL(false, detail::From_Ruby<bool>().convert(Qfalse));
  ASSERT_EQUAL(false, detail::From_Ruby<bool>().convert(Qnil));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(Qtrue));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_str_new2("some string")));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(INT2NUM(3)));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_float_new(3.33)));
}

TESTCASE(float_from_ruby)
{
  ASSERT_EQUAL(0.0f, detail::From_Ruby<float>().convert(rb_float_new(0.0f)));
  ASSERT_EQUAL(-1.0f, detail::From_Ruby<float>().convert(rb_float_new(-1.0f)));
  ASSERT_EQUAL(1.0f, detail::From_Ruby<float>().convert(rb_float_new(1.0f)));
  ASSERT_EQUAL(std::numeric_limits<float>::min(),
               detail::From_Ruby<float>().convert(rb_float_new(std::numeric_limits<float>::min())));
  ASSERT_EQUAL(std::numeric_limits<float>::max(),
               detail::From_Ruby<float>().convert(rb_float_new(std::numeric_limits<float>::max())));
  ASSERT(std::isnan(detail::From_Ruby<float>().convert(rb_float_new(std::numeric_limits<float>::quiet_NaN()))));
  ASSERT(std::isnan(detail::From_Ruby<float>().convert(rb_float_new(std::numeric_limits<float>::signaling_NaN()))));
  ASSERT_EQUAL(std::numeric_limits<float>::epsilon(),
               detail::From_Ruby<float>().convert(rb_float_new(std::numeric_limits<float>::epsilon())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<float>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion to float from string", ex.what())
  );
}

TESTCASE(double_from_ruby)
{
  ASSERT_EQUAL(0.0, detail::From_Ruby<double>().convert(rb_float_new(0.0)));
  ASSERT_EQUAL(-1.0, detail::From_Ruby<double>().convert(rb_float_new(-1.0)));
  ASSERT_EQUAL(1.0, detail::From_Ruby<double>().convert(rb_float_new(1.0)));
  ASSERT_EQUAL(std::numeric_limits<double>::min(),
               detail::From_Ruby<double>().convert(rb_float_new(std::numeric_limits<double>::min())));
  ASSERT_EQUAL(std::numeric_limits<double>::max(),
               detail::From_Ruby<double>().convert(rb_float_new(std::numeric_limits<double>::max())));
  ASSERT(std::isnan(detail::From_Ruby<double>().convert(rb_float_new(std::numeric_limits<double>::quiet_NaN()))));
  ASSERT(std::isnan(detail::From_Ruby<double>().convert(rb_float_new(std::numeric_limits<double>::signaling_NaN()))));
  ASSERT_EQUAL(std::numeric_limits<double>::epsilon(),
               detail::From_Ruby<double>().convert(rb_float_new(std::numeric_limits<double>::epsilon())));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<double>().convert(rb_str_new2("bad value")),
    ASSERT_EQUAL("no implicit conversion to float from string", ex.what())
  );
}*/

namespace
{
  std::string floatPointer(float* buffer, int size)
  {
    std::ostringstream result;
    result << "[";

    for (int i = 0; i < size; i++)
    {
      result << buffer[i];
      if (i < size - 1)
      {
        result << ", ";
      }
    }
    result << "]";
    return result.str();
  }

  std::string doublePointer(double* buffer, int size)
  {
    std::ostringstream result;
    result << "[";

    for (int i = 0; i < size; i++)
    {
      result << buffer[i];
      if (i < size - 1)
      {
        result << ", ";
      }
    }
    result << "]";
    return result.str();
  }

  std::string intPointer(int* buffer, int size)
  {
    std::ostringstream result;
    result << "[";

    for (int i = 0; i < size; i++)
    {
      result << buffer[i];
      if (i < size - 1)
      {
        result << ", ";
      }
    }
    result << "]";
    return result.str();
  }
}

TESTCASE(float_pointer_from_ruby)
{
  Module m = define_module("Testing");
  m.define_singleton_function("float_pointer", floatPointer);

  std::string code = R"(arr = [4.3, 3.2, 2.1, 1.1]
                        float_pointer(arr, arr.size))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[4.3, 3.2, 2.1, 1.1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4, 3, 2.8, 1]
            float_pointer(arr, arr.size))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2.8, 1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4, "bad", 2, 1]
            float_pointer(arr, arr.size))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("no implicit conversion to float from string", ex.what())
  );
}

/*TESTCASE(double_pointer_from_ruby)
{
  Module m = define_module("Testing");
  m.define_singleton_function("double_pointer", doublePointer);

  std::string code = R"(arr = [1.1, 2.2, 3.3, 4.4]
                        double_pointer(arr, arr.size))";

  Object result = m.module_eval(code);

  ASSERT_EQUAL("[1.1, 2.2, 3.3, 4.4]", detail::From_Ruby<std::string>().convert(result.value()));
}

TESTCASE(int_pointer_from_ruby)
{
  Module m = define_module("Testing");
  m.define_singleton_function("int_pointer", intPointer);

  std::string code = R"(arr = [4, 3, 2, 1]
                        int_pointer(arr, arr.size))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2, 1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4.2, 3.8, 2.0, 1]
            int_pointer(arr, arr.size))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2, 1]", detail::From_Ruby<std::string>().convert(result.value()));
}

TESTCASE(char_const_ptr_from_ruby)
{
  char const* foo = "foo";
  ASSERT_EQUAL(foo, detail::From_Ruby<char const *>().convert(rb_str_new2("foo")));
  ASSERT_EQUAL("", detail::From_Ruby<char const*>().convert(rb_str_new2("")));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<char const *>().convert(rb_float_new(32.3)),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}

TESTCASE(char_from_ruby_single_character_string)
{
  ASSERT_EQUAL('x', detail::From_Ruby<char>().convert(rb_str_new2("x")));
}

TESTCASE(char_from_ruby_longer_string)
{
  ASSERT_EXCEPTION_CHECK(
    std::invalid_argument,
    detail::From_Ruby<char>().convert(rb_str_new2("xy")),
    ASSERT_EQUAL("from_ruby<char>: string must have length 1", ex.what())
  );

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<char>().convert(rb_float_new(47.2)),
    ASSERT_EQUAL("wrong argument type Float (expected char type)", ex.what())
  );
}

TESTCASE(char_from_ruby_fixnum)
{
  ASSERT_EQUAL('1', detail::From_Ruby<char>().convert(INT2NUM(49)));
}

TESTCASE(unsigned_char_from_ruby)
{
  unsigned char expected = -1;
  ASSERT_EQUAL(expected, detail::From_Ruby<unsigned char>().convert(INT2NUM(-1)));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<unsigned char>().convert(rb_float_new(1.3)),
    ASSERT_EQUAL("wrong argument type Float (expected char type)", ex.what())
  );
}

TESTCASE(signed_char_from_ruby)
{
  unsigned char expected = 10;
  ASSERT_EQUAL(expected, detail::From_Ruby<signed char>().convert(INT2NUM(10)));
}

TESTCASE(char_star_from_ruby)
{
  const char* expected = "my string";
  ASSERT_EQUAL(expected, detail::From_Ruby<const char*>().convert(rb_str_new2("my string")));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<const char*>().convert(rb_float_new(11.11)),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}
*/