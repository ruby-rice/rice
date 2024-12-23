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

namespace
{
  template<typename T>
  std::string toString(T* buffer, int size)
  {
    std::ostringstream result;
    result << "[";

    for (int i = 0; i < size; i++)
    {
      T item = buffer[i];
      result << item;
      if (i < size - 1)
      {
        result << ", ";
      }
    }
    result << "]";
    return result.str();
  }
}

TESTCASE(bool)
{
  ASSERT_EQUAL(detail::Convertible::TypeCast, detail::From_Ruby<bool>().is_convertible(Qfalse));
  ASSERT_EQUAL(detail::Convertible::Exact, detail::From_Ruby<bool>().is_convertible(Qtrue));
  ASSERT_EQUAL(detail::Convertible::TypeCast, detail::From_Ruby<bool>().is_convertible(Qnil));
  ASSERT_EQUAL(detail::Convertible::None, detail::From_Ruby<bool>().is_convertible(INT2NUM(3)));
  ASSERT_EQUAL(detail::Convertible::None, detail::From_Ruby<bool>().is_convertible(rb_float_new(3.33)));
  ASSERT_EQUAL(detail::Convertible::None, detail::From_Ruby<bool>().is_convertible(rb_str_new2("some string")));

  ASSERT_EQUAL(false, detail::From_Ruby<bool>().convert(Qfalse));
  ASSERT_EQUAL(false, detail::From_Ruby<bool>().convert(Qnil));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(Qtrue));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(INT2NUM(3)));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(INT2NUM(0)));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_float_new(3.33)));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_float_new(0.0)));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_str_new2("some string")));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_str_new2("true")));
  ASSERT_EQUAL(true, detail::From_Ruby<bool>().convert(rb_str_new2("false")));
}

TESTCASE(char)
{
  unsigned char expected = 10;
  ASSERT_EQUAL(expected, detail::From_Ruby<signed char>().convert(INT2NUM(10)));
}

TESTCASE(char_single_character_string)
{
  ASSERT_EQUAL('x', detail::From_Ruby<char>().convert(rb_str_new2("x")));
}

TESTCASE(char_longer_string)
{
  ASSERT_EQUAL('x', detail::From_Ruby<char>().convert(rb_str_new2("xy")));
}

TESTCASE(char_float)
{
  // '/' is 47
  ASSERT_EQUAL('/', detail::From_Ruby<char>().convert(rb_float_new(47.2)));
}

TESTCASE(char_fixnum)
{
  // '1' is 49
  ASSERT_EQUAL('1', detail::From_Ruby<char>().convert(INT2NUM(49)));
}

TESTCASE(char_pointer)
{
  Module m = define_module("Testing");
  detail::From_Ruby<char*> fromRuby;

  char* expected = nullptr;
  char* result = fromRuby.convert(Qnil);
  ASSERT_EQUAL(expected, result);

  const char* expected2 = "my string";
  result = fromRuby.convert(rb_str_new2(expected2));
  ASSERT_EQUAL(*expected2, *result);

  std::string code = R"(arr = [0, 127, 128, 255, 256, -128, -129, -255])";
  Array array = m.instance_eval(code);
  result = fromRuby.convert(array.value());

  ASSERT_EQUAL(result[0], 0);
  ASSERT_EQUAL(result[1], 127);
  ASSERT_EQUAL(result[2], -128);
  ASSERT_EQUAL(result[3], -1);
  ASSERT_EQUAL(result[4], 0);
  ASSERT_EQUAL(result[5], -128);
  ASSERT_EQUAL(result[6], 127);
  ASSERT_EQUAL(result[7], 1);

  std::string format = CHAR_MIN < 0 ? "c*" : "C*";
  String packed = array.call("pack", format);
  result = fromRuby.convert(packed.value());

  ASSERT_EQUAL(result[0], 0);
  ASSERT_EQUAL(result[1], 127);
  ASSERT_EQUAL(result[2], -128);
  ASSERT_EQUAL(result[3], -1);
  ASSERT_EQUAL(result[4], 0);
  ASSERT_EQUAL(result[5], -128);
  ASSERT_EQUAL(result[6], 127);
  ASSERT_EQUAL(result[7], 1);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    fromRuby.convert(rb_float_new(11.11)),
    ASSERT_EQUAL("wrong argument type Float (expected char*)", ex.what())
  );
}

TESTCASE(signed_char_pointer)
{
  Module m = define_module("Testing");
  detail::From_Ruby<signed char*> fromRuby;

  signed char* expected = nullptr;
  signed char* result = fromRuby.convert(Qnil);
  ASSERT_EQUAL(expected, result);

  expected = (signed char*)"my string";
  result = fromRuby.convert(rb_str_new2("my string"));
  ASSERT_EQUAL(*expected, *result);

  std::string code = R"(arr = [0, 127, 128, 255, 256, -128, -129, -255])";
  Array array = m.instance_eval(code);
  result = fromRuby.convert(array.value());

  ASSERT_EQUAL(result[0], 0);
  ASSERT_EQUAL(result[1], 127);
  ASSERT_EQUAL(result[2], -128);
  ASSERT_EQUAL(result[3], -1);
  ASSERT_EQUAL(result[4], 0);
  ASSERT_EQUAL(result[5], -128);
  ASSERT_EQUAL(result[6], 127);
  ASSERT_EQUAL(result[7], 1);

  std::string format = CHAR_MIN < 0 ? "c*" : "C*";
  String packed = array.call("pack", format);
  result = fromRuby.convert(packed.value());

  ASSERT_EQUAL(result[0], 0);
  ASSERT_EQUAL(result[1], 127);
  ASSERT_EQUAL(result[2], -128);
  ASSERT_EQUAL(result[3], -1);
  ASSERT_EQUAL(result[4], 0);
  ASSERT_EQUAL(result[5], -128);
  ASSERT_EQUAL(result[6], 127);
  ASSERT_EQUAL(result[7], 1);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    fromRuby.convert(rb_float_new(11.11)),
    ASSERT_EQUAL("wrong argument type Float (expected signed char*)", ex.what())
  );
}

TESTCASE(char_pointer_const)
{
  char const* foo = "foo";
  ASSERT_EQUAL(foo, detail::From_Ruby<char const*>().convert(rb_str_new2("foo")));
  ASSERT_EQUAL("", detail::From_Ruby<char const*>().convert(rb_str_new2("")));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<char const*>().convert(rb_float_new(32.3)),
    ASSERT_EQUAL("wrong argument type Float (expected char*)", ex.what())
  );
}

TESTCASE(unsigned_char)
{
  unsigned char expected = -1;
  ASSERT_EQUAL(expected, detail::From_Ruby<unsigned char>().convert(INT2NUM(-1)));

  expected = 1;
  ASSERT_EQUAL(expected, detail::From_Ruby<unsigned char>().convert(rb_float_new(1.3)));
}

TESTCASE(unsigned_char_pointer)
{
  Module m = define_module("Testing");

  detail::From_Ruby<unsigned char*> fromRuby;

  unsigned char* expected = nullptr;
  unsigned char* result = fromRuby.convert(Qnil);
  ASSERT_EQUAL(expected, result);

  std::string code = R"(arr = [0, 127, 128, 255, 256, -128, -129, -255])";
  Array array = m.instance_eval(code);
  result = fromRuby.convert(array.value());

  ASSERT_EQUAL(result[0], 0x0);
  ASSERT_EQUAL(result[1], 0x7F);
  ASSERT_EQUAL(result[2], 0x80);
  ASSERT_EQUAL(result[3], 0xFF);
  ASSERT_EQUAL(result[4], 0x00);
  ASSERT_EQUAL(result[5], 0x80);
  ASSERT_EQUAL(result[6], 0x7F);
  ASSERT_EQUAL(result[7], 0x01);

  String packed = array.call("pack", "C*");
  result = fromRuby.convert(packed.value());

  ASSERT_EQUAL(result[0], 0x0);
  ASSERT_EQUAL(result[1], 0x7F);
  ASSERT_EQUAL(result[2], 0x80);
  ASSERT_EQUAL(result[3], 0xFF);
  ASSERT_EQUAL(result[4], 0x00);
  ASSERT_EQUAL(result[5], 0x80);
  ASSERT_EQUAL(result[6], 0x7F);
  ASSERT_EQUAL(result[7], 0x01);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<const char*>().convert(rb_float_new(11.11)),
    ASSERT_EQUAL("wrong argument type Float (expected char*)", ex.what())
  );
}

TESTCASE(unsigned_char_pointer_array)
{
  Module m = define_module("Testing");

  m.define_singleton_function("unsigned_char_pointer", &toString<unsigned char>);

  std::string code = R"(arr = ["A", "B"]
                        unsigned_char_pointer(arr, arr.size))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Object result = m.module_eval(code),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );

  code = R"(arr = ["A", "B"].map do |char|
                               char.ord
                             end  
            unsigned_char_pointer(arr, arr.size))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL("[A, B]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [65, 66]
            unsigned_char_pointer(arr, arr.size))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[A, B]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [true]
            unsigned_char_pointer(arr, arr.size))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("no implicit conversion of true into Integer", ex.what())
  );
}

TESTCASE(double)
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
}

TESTCASE(double_pointer_array)
{
  Module m = define_module("Testing");
  m.define_singleton_function("double_pointer", &toString<double>);

  std::string code = R"(arr = [1.1, 2.2, 3.3, 4.4]
                        double_pointer(arr, arr.size))";

  Object result = m.module_eval(code);

  ASSERT_EQUAL("[1.1, 2.2, 3.3, 4.4]", detail::From_Ruby<std::string>().convert(result.value()));
}

TESTCASE(float)
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

TESTCASE(float_pointer_array)
{
  Module m = define_module("Testing");

  m.define_singleton_function("float_pointer", &toString<float>);

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
    ASSERT_EQUAL("can't convert String into Float", ex.what())
  );
}

TESTCASE(int)
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

TESTCASE(int_pointer_array)
{
  Module m = define_module("Testing");
  m.define_singleton_function("int_pointer", &toString<int>);

  std::string code = R"(arr = [4, 3, 2, 1]
                        int_pointer(arr, arr.size))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2, 1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4.2, 3.8, 2.0, 1]
            int_pointer(arr, arr.size))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2, 1]", detail::From_Ruby<std::string>().convert(result.value()));
}

TESTCASE(unsigned_int)
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

TESTCASE(long)
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

TESTCASE(unsigned_long)
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

TESTCASE(long_long)
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

TESTCASE(unsigned_long_long)
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

TESTCASE(object)
{
  Object o(rb_str_new2("foo"));
  ASSERT_EQUAL(o, detail::From_Ruby<Object>().convert(o));
}

TESTCASE(short)
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

TESTCASE(unsigned_short)
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
