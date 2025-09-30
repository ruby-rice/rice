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
  std::string arrayToString(T* buffer, int size)
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

  template<typename T>
  std::string voidToString(void* buffer, int size)
  {
    return arrayToString<T>((T*)buffer, size/sizeof(T));
  }

  template<typename T>
  std::string arrayofArraysToString(T** buffer, int outerSize, int innerSize)
  {
    std::ostringstream result;
    result << "[";

    for (int i = 0; i < outerSize; i++)
    {
      T* outer = buffer[i];

      result << "[";
      for (int j = 0; j < innerSize; j++)
      {
        T item = outer[j];
        result << item;
        if (j < innerSize - 1)
        {
          result << ", ";
        }
      }
      result << "]";
      if (i < outerSize - 1)
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
  ASSERT_EQUAL(detail::Convertible::Exact, detail::From_Ruby<bool>().is_convertible(Qfalse));
  ASSERT_EQUAL(detail::Convertible::Exact, detail::From_Ruby<bool>().is_convertible(Qtrue));
  ASSERT_EQUAL(detail::Convertible::Cast, detail::From_Ruby<bool>().is_convertible(Qnil));
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
  define_pointer<char>();
  define_buffer<char>();
  detail::From_Ruby<char*> fromRuby;

  char* buffer = fromRuby.convert(Qnil);
  ASSERT_EQUAL(nullptr, buffer);

  std::string code = R"(Rice::Buffer≺char≻.new("my string"))";
  Object result = m.instance_eval(code);
  Object data = result.call("data");
  buffer = fromRuby.convert(data);
  const char* expected = "my string";
  ASSERT_EQUAL(*expected, *buffer);

  code = R"(Rice::Buffer≺char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  result = m.instance_eval(code);
  data = result.call("data");
  buffer = fromRuby.convert(data);

  ASSERT_EQUAL(buffer[0], 0);
  ASSERT_EQUAL(buffer[1], 127);
  ASSERT_EQUAL(buffer[2], -128);
  ASSERT_EQUAL(buffer[3], -1);
  ASSERT_EQUAL(buffer[4], 0);
  ASSERT_EQUAL(buffer[5], -128);
  ASSERT_EQUAL(buffer[6], 127);
  ASSERT_EQUAL(buffer[7], 1);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    fromRuby.convert(rb_float_new(11.11)),
    ASSERT_EQUAL("wrong argument type Float (expected Pointer≺char≻)", ex.what())
  );
}

TESTCASE(signed_char_pointer)
{
  Module m = define_module("Testing");
  define_buffer<signed char>();
  detail::From_Ruby<signed char*> fromRuby;

  signed char* expected = nullptr;
  signed char* buffer = fromRuby.convert(Qnil);
  ASSERT_EQUAL(expected, buffer);

  std::string code = R"(Rice::Buffer≺signed char≻.new("my string"))";
  Object result = m.instance_eval(code);
  Object data = result.call("data");
  buffer = fromRuby.convert(data);
  expected = (signed char*)"my string";
  ASSERT_EQUAL(*expected, *buffer);

  code = R"(Rice::Buffer≺signed char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  result = m.instance_eval(code);
  data = result.call("data");
  buffer = fromRuby.convert(data);

  ASSERT_EQUAL(buffer[0], 0);
  ASSERT_EQUAL(buffer[1], 127);
  ASSERT_EQUAL(buffer[2], -128);
  ASSERT_EQUAL(buffer[3], -1);
  ASSERT_EQUAL(buffer[4], 0);
  ASSERT_EQUAL(buffer[5], -128);
  ASSERT_EQUAL(buffer[6], 127);
  ASSERT_EQUAL(buffer[7], 1);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    fromRuby.convert(rb_float_new(11.11)),
    ASSERT_EQUAL("Wrong argument type. Expected: Rice::Pointer≺signed char≻. Received: Float.", ex.what())
  );
}

TESTCASE(char_pointer_const)
{
  Module m = define_module("Testing");
  define_pointer<char>();

  std::string code = R"(Rice::Buffer≺char≻.new("my string"))";
  Object result = m.instance_eval(code);
  const char* expected = "my string";
  const char* data = detail::From_Ruby<char*>().convert(result.call("data"));
  ASSERT_EQUAL(expected, data);

  code = R"(Rice::Buffer≺char≻.new(""))";
  result = m.instance_eval(code);
  expected = "";
  data = detail::From_Ruby<char*>().convert(result.call("data"));
  ASSERT_EQUAL(expected, data);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<char*>().convert(rb_float_new(32.3)),
    ASSERT_EQUAL("wrong argument type Float (expected Pointer≺char≻)", ex.what())
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
  define_buffer<unsigned char>();
  detail::From_Ruby<Buffer<unsigned char>*> fromRuby;

  Buffer<unsigned char>* buffer = fromRuby.convert(Qnil);
  ASSERT_EQUAL(nullptr, buffer);

  std::string code = R"(Rice::Buffer≺unsigned char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  Object result = m.instance_eval(code);
  buffer = fromRuby.convert(result.value());

  ASSERT_EQUAL((size_t)8, buffer->size());
  ASSERT_EQUAL(buffer->operator[](0), 0x0);
  ASSERT_EQUAL(buffer->operator[](1), 0x7F);
  ASSERT_EQUAL(buffer->operator[](2), 0x80);
  ASSERT_EQUAL(buffer->operator[](3), 0xFF);
  ASSERT_EQUAL(buffer->operator[](4), 0x00);
  ASSERT_EQUAL(buffer->operator[](5), 0x80);
  ASSERT_EQUAL(buffer->operator[](6), 0x7F);
  ASSERT_EQUAL(buffer->operator[](7), 0x01);

  code = R"(array = [0, 127, 128, 255, 256, -128, -129, -255]
            packed = array.pack("C*")
            Rice::Buffer≺unsigned char≻.new(packed))";
  result = m.instance_eval(code);
  buffer = fromRuby.convert(result.value());

  ASSERT_EQUAL((size_t)8, buffer->size());
  ASSERT_EQUAL(buffer->operator[](0), 0x0);
  ASSERT_EQUAL(buffer->operator[](1), 0x7F);
  ASSERT_EQUAL(buffer->operator[](2), 0x80);
  ASSERT_EQUAL(buffer->operator[](3), 0xFF);
  ASSERT_EQUAL(buffer->operator[](4), 0x00);
  ASSERT_EQUAL(buffer->operator[](5), 0x80);
  ASSERT_EQUAL(buffer->operator[](6), 0x7F);
  ASSERT_EQUAL(buffer->operator[](7), 0x01);

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<const char*>().convert(rb_float_new(11.11)),
    ASSERT_EQUAL("Wrong argument type. Expected: Rice::Pointer≺char≻. Received: Float.", ex.what())
  );
}

TESTCASE(unsigned_char_pointer_array)
{
  Module m = define_module("Testing");

  m.define_singleton_function("unsigned_char_pointer", &arrayToString<unsigned char>);

  std::string code = R"(arr = ["A", "B"]
                        buffer = Rice::Buffer≺unsigned char≻.new(arr)
                        unsigned_char_pointer(buffer.data, buffer.size))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    Object result = m.module_eval(code),
    ASSERT_EQUAL("no implicit conversion of String into Integer", ex.what())
  );

  code = R"(arr = ["A", "B"].map do |char|
                               char.ord
                             end  
            buffer = Rice::Buffer≺unsigned char≻.new(arr)
            unsigned_char_pointer(buffer.data, buffer.size))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL("[A, B]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [65, 66]
            buffer = Rice::Buffer≺unsigned char≻.new(arr)
            unsigned_char_pointer(buffer.data, buffer.size))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[A, B]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [true]
            buffer = Rice::Buffer≺unsigned char≻.new(arr)
            unsigned_char_pointer(buffer.ptr, buffer.size))";

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
  m.define_singleton_function("double_pointer", &arrayToString<double>);

  std::string code = R"(arr = [1.1, 2.2, 3.3, 4.4]
                        buffer = Rice::Buffer≺double≻.new(arr)
                        double_pointer(buffer.data, buffer.size))";

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

  m.define_singleton_function("float_pointer", &arrayToString<float>);

  std::string code = R"(arr = [4.3, 3.2, 2.1, 1.1]
                        buffer = Rice::Buffer≺float≻.new(arr)
                        float_pointer(buffer.data, buffer.size))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[4.3, 3.2, 2.1, 1.1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4, 3, 2.8, 1]
           buffer = Rice::Buffer≺float≻.new(arr)
            float_pointer(buffer.data, buffer.size))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2.8, 1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4, "bad", 2, 1]
            buffer = Rice::Buffer≺float≻.new(arr)
            float_pointer(buffer.data, buffer.size))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("can't convert String into Float", ex.what())
  );
}

TESTCASE(float_array_array)
{
  Module m = define_module("Testing");

  m.define_singleton_function("array_array_to_string", &arrayofArraysToString<float>);

  std::string code = R"(buffer = Rice::Buffer≺float∗≻.new([[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]])
                        array_array_to_string(buffer.data, buffer.size, 2))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(buffer = Rice::Buffer≺float∗≻.new([[4, 3], [2.8, 1]])
            array_array_to_string(buffer.data, buffer.size, 2))";
  result = m.module_eval(code);
  ASSERT_EQUAL("[[4, 3], [2.8, 1]]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(buffer = Rice::Buffer≺float∗≻.new([[4, "bad"], [2, 1]])
            array_array_to_string(buffer.data, buffer.size, 2))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("can't convert String into Float", ex.what())
  );

  code = R"(array = [[4, "bad"], [2, 1]]
            array_array_to_string(array, array.size, 2))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("Wrong argument type. Expected: Rice::Pointer≺float∗≻. Received: Array.", ex.what())
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
  m.define_singleton_function("int_pointer", &arrayToString<int>);

  std::string code = R"(arr = [4, 3, 2, 1]
                        buffer = Rice::Buffer≺int≻.new(arr)
                        int_pointer(buffer.data, buffer.size))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2, 1]", detail::From_Ruby<std::string>().convert(result.value()));

  code = R"(arr = [4.2, 3.8, 2.0, 1]
            buffer = Rice::Buffer≺int≻.new(arr)
            int_pointer(buffer.data, buffer.size))";
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

TESTCASE(void_pointer_array)
{
  Module m = define_module("Testing");
  m.define_singleton_function("void_pointer", &voidToString<int>);

  std::string code = R"(arr = [4, 3, 2, 1]
                        data = arr.pack("i*")
                        void_pointer(data, data.size))";
  Object result = m.module_eval(code);
  ASSERT_EQUAL("[4, 3, 2, 1]", detail::From_Ruby<std::string>().convert(result.value()));
}
