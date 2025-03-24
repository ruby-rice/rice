#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <limits>
#include <cmath>

using namespace Rice;

TESTSUITE(Buffer);

SETUP(Buffer)
{
  embed_ruby();
}

TEARDOWN(Buffer)
{
  rb_gc_start();
}

TESTCASE(Char)
{
  define_buffer<Buffer<char>>();
  detail::From_Ruby<char*> fromRuby;

  Module m = define_module("BufferTesting");

  std::string code = u8R"(Rice::Buffer≺char≻.new("my string"))";
  Object result = m.instance_eval(code);

  ASSERT_EQUAL(u8"Rice::Buffer≺char≻", result.class_name().c_str());
  ASSERT_EQUAL(u8"my string", String(result.call("read")).c_str());

  Array array = result.call("to_a");
  ASSERT_EQUAL("109, 121, 32, 115, 116, 114, 105, 110, 103", array.join(", ").c_str());

  Object size = result.call("size");
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(size));

  Data_Object<Buffer<char>> dataObject(result);
  Buffer<char> buffer = std::move(*dataObject);
  ASSERT_EQUAL("my string", buffer.get());
  ASSERT_EQUAL(9, buffer.size());
}

TESTCASE(CharArray)
{
  define_buffer<Buffer<char>>();

  Module m = define_module("BufferTesting");

  std::string code = u8R"(Rice::Buffer≺char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  Data_Object<Buffer<char>> result = m.instance_eval(code);
  Buffer<char> buffer = std::move(*result);
  char* data = buffer.get();

  ASSERT_EQUAL(data[0], 0);
  ASSERT_EQUAL(data[1], 127);
  ASSERT_EQUAL(data[2], -128);
  ASSERT_EQUAL(data[3], -1);
  ASSERT_EQUAL(data[4], 0);
  ASSERT_EQUAL(data[5], -128);
  ASSERT_EQUAL(data[6], 127);
  ASSERT_EQUAL(data[7], 1);
}

TESTCASE(signed_char_pointer)
{
  define_buffer<Buffer<signed char>>();
  Module m = define_module("Testing");

  std::string code = u8R"(Rice::Buffer≺signed char≻.new("my string"))";
  Data_Object<Buffer<signed char>> result = m.instance_eval(code);
  Buffer<signed char> buffer = std::move(*result);
  signed char* data = buffer.get();

  signed char* expected = (signed char*)"my string";
  ASSERT_EQUAL(*expected, *data);

  code = u8R"(Rice::Buffer≺signed char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  result = m.instance_eval(code);
  buffer = std::move(*result);
  data = buffer.get();

  ASSERT_EQUAL(data[0], 0);
  ASSERT_EQUAL(data[1], 127);
  ASSERT_EQUAL(data[2], -128);
  ASSERT_EQUAL(data[3], -1);
  ASSERT_EQUAL(data[4], 0);
  ASSERT_EQUAL(data[5], -128);
  ASSERT_EQUAL(data[6], 127);
  ASSERT_EQUAL(data[7], 1);
}

TESTCASE(char_pointer_const)
{
  define_buffer<Buffer<char>>();
  Module m = define_module("Testing");

  std::string code = u8R"(Rice::Buffer≺char≻.new("my string"))";

  Data_Object<Buffer<char>> result = m.instance_eval(code);
  Buffer<char> buffer = std::move(*result);
  char* data = buffer.get();

  const char* expected = "my string";
  ASSERT_EQUAL(expected, data);

  code = R"(Rice::Buffer≺char≻.new(""))";
  result = m.instance_eval(code);
  buffer = std::move(*result);
  data = buffer.get();

  expected = "";
  ASSERT_EQUAL(expected, data);
}

TESTCASE(unsigned_char_pointer)
{
  define_buffer<Buffer<unsigned char>>();
  Module m = define_module("Testing");

  std::string code = u8R"(Rice::Buffer≺unsigned char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  Data_Object<Buffer<unsigned char>> result = m.instance_eval(code);
  Buffer<unsigned char> buffer = std::move(*result);
  unsigned char* data = buffer.get();

  ASSERT_EQUAL(data[0], 0x0);
  ASSERT_EQUAL(data[1], 0x7F);
  ASSERT_EQUAL(data[2], 0x80);
  ASSERT_EQUAL(data[3], 0xFF);
  ASSERT_EQUAL(data[4], 0x00);
  ASSERT_EQUAL(data[5], 0x80);
  ASSERT_EQUAL(data[6], 0x7F);
  ASSERT_EQUAL(data[7], 0x01);

  code = u8R"(array = [0, 127, 128, 255, 256, -128, -129, -255]
              packed = array.pack("C*")
              Rice::Buffer≺unsigned char≻.new(packed))";
  result = m.instance_eval(code);
  buffer = std::move(*result);
  data = buffer.get();

  ASSERT_EQUAL(data[0], 0x0);
  ASSERT_EQUAL(data[1], 0x7F);
  ASSERT_EQUAL(data[2], 0x80);
  ASSERT_EQUAL(data[3], 0xFF);
  ASSERT_EQUAL(data[4], 0x00);
  ASSERT_EQUAL(data[5], 0x80);
  ASSERT_EQUAL(data[6], 0x7F);
  ASSERT_EQUAL(data[7], 0x01);
}

TESTCASE(float_array_array)
{
  define_buffer<Buffer<float*>>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺float∗≻.new([[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]]))";
  Data_Object<Buffer<float*>> result = m.instance_eval(code);
  Buffer<float*> buffer = std::move(*result);

  ASSERT_EQUAL(3, buffer.size());

  const Buffer<float>& bufferInner1 = buffer[0];
  ASSERT_EQUAL(2, bufferInner1.size());

  const Buffer<float>& bufferInner2 = buffer[1];
  ASSERT_EQUAL(2, bufferInner2.size());

  const Buffer<float>& bufferInner3 = buffer[2];
  ASSERT_EQUAL(2, bufferInner3.size());
}

TESTCASE(wrong_type)
{
  define_buffer<Buffer<float*>>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺float∗≻.new([[4, "bad"], [2, 1]]))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("can't convert String into Float", ex.what())
  );
}
