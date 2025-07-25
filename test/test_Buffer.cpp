#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

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
  define_buffer<char>();

  Module m = define_module("BufferTesting");

  std::string code = R"(Rice::Buffer≺char≻.new("my string"))";
  Object result = m.instance_eval(code);

  ASSERT_EQUAL("Rice::Buffer≺char≻", result.class_name().c_str());
  ASSERT_EQUAL("my string", String(result.call("bytes")).c_str());

  Array array = result.call("to_ary");
  ASSERT_EQUAL("109, 121, 32, 115, 116, 114, 105, 110, 103", array.join(", ").c_str());

  Object size = result.call("size");
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(size));

  Data_Object<Buffer<char>> dataObject(result);
  Buffer<char> buffer = std::move(*dataObject);
  ASSERT_EQUAL("my string", buffer.ptr());
  ASSERT_EQUAL(9, (int)buffer.size());
}

TESTCASE(CharArray)
{
  define_buffer<char>();

  Module m = define_module("BufferTesting");

  std::string code = R"(Rice::Buffer≺char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  Data_Object<Buffer<char>> result = m.instance_eval(code);
  Buffer<char> buffer = std::move(*result);
  char* data = buffer.ptr();

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
  define_buffer<signed char>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺signed char≻.new("my string"))";
  Data_Object<Buffer<signed char>> result = m.instance_eval(code);
  Buffer<signed char> buffer = std::move(*result);
  signed char* data = buffer.ptr();

  signed char* expected = (signed char*)"my string";
  ASSERT_EQUAL(*expected, *data);

  code = R"(Rice::Buffer≺signed char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  result = m.instance_eval(code);
  buffer = std::move(*result);
  data = buffer.ptr();

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
  define_buffer<char>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺char≻.new("my string"))";

  Data_Object<Buffer<char>> result = m.instance_eval(code);
  Buffer<char> buffer = std::move(*result);
  char* data = buffer.ptr();

  const char* expected = "my string";
  ASSERT_EQUAL(expected, data);

  code = R"(Rice::Buffer≺char≻.new(""))";
  result = m.instance_eval(code);
  buffer = std::move(*result);
  data = buffer.ptr();

  expected = "";
  ASSERT_EQUAL(expected, data);
}

TESTCASE(unsigned_char_pointer)
{
  define_buffer<unsigned char>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺unsigned char≻.new([0, 127, 128, 255, 256, -128, -129, -255]))";
  Data_Object<Buffer<unsigned char>> result = m.instance_eval(code);
  Buffer<unsigned char> buffer = std::move(*result);
  unsigned char* data = buffer.ptr();

  ASSERT_EQUAL(data[0], 0x0);
  ASSERT_EQUAL(data[1], 0x7F);
  ASSERT_EQUAL(data[2], 0x80);
  ASSERT_EQUAL(data[3], 0xFF);
  ASSERT_EQUAL(data[4], 0x00);
  ASSERT_EQUAL(data[5], 0x80);
  ASSERT_EQUAL(data[6], 0x7F);
  ASSERT_EQUAL(data[7], 0x01);

  code = R"(array = [0, 127, 128, 255, 256, -128, -129, -255]
              packed = array.pack("C*")
              Rice::Buffer≺unsigned char≻.new(packed))";
  result = m.instance_eval(code);
  buffer = std::move(*result);
  data = buffer.ptr();

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
  define_buffer<float*>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺float∗≻.new([[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]]))";
  Data_Object<Buffer<float*>> result = m.instance_eval(code);
  Buffer<float*> buffer = std::move(*result);

  ASSERT_EQUAL(3, (int)buffer.size());

  const Buffer<float>& bufferInner1 = buffer[0];
  ASSERT_EQUAL(2, (int)bufferInner1.size());

  const Buffer<float>& bufferInner2 = buffer[1];
  ASSERT_EQUAL(2, (int)bufferInner2.size());

  const Buffer<float>& bufferInner3 = buffer[2];
  ASSERT_EQUAL(2, (int)bufferInner3.size());
}

TESTCASE(wrong_type)
{
  define_buffer<float*>();
  Module m = define_module("Testing");

  std::string code = R"(Rice::Buffer≺float∗≻.new([[4, "bad"], [2, 1]]))";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("can't convert String into Float", ex.what())
  );
}

namespace
{
  void updateRef(int& ref)
  {
    ref = 4;
  }

  void updatePtr(int* ptr)
  {
    *ptr = 5;
  }
}

TESTCASE(update_reference)
{
  define_buffer<int>();
  Module m = define_module("Testing");
  m.define_module_function("update_reference", &updateRef);

  std::string code = R"(buffer = Rice::Buffer≺int≻.new(0)
                        update_reference(buffer)
                        buffer.to_ary(1).first)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            update_reference(buffer)
            buffer[0])";

  result = m.module_eval(code);
  ASSERT_EQUAL(4, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            update_reference(buffer)
            buffer[1])";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("index 1 outside of bounds: 0..1", ex.what())
  );
}

TESTCASE(update_ptr)
{
  define_buffer<int>();
  Module m = define_module("Testing");
  m.define_module_function("update_pointer", &updatePtr);

  std::string code = R"(buffer = Rice::Buffer≺int≻.new(0)
                        update_pointer(buffer)
                        buffer.to_ary(1).first)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            update_pointer(buffer)
            buffer[0])";

  result = m.module_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            update_pointer(buffer)
            buffer[1])";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("index 1 outside of bounds: 0..1", ex.what())
  );
}

TESTCASE(update_buffer)
{
  define_buffer<int>();
  Module m = define_module("Testing");

  std::string code = R"(buffer = Rice::Buffer≺int≻.new(0)
                        buffer[0] = 8
                        buffer[0])";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(8, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            buffer[1] = 9
            buffer[1])";

  ASSERT_EXCEPTION_CHECK(
    Exception,
    m.module_eval(code),
    ASSERT_EQUAL("index 1 outside of bounds: 0..1", ex.what())
  );
}

TESTCASE(to_s)
{
  define_buffer<unsigned char>();
  Module m = define_module("Testing");

  std::string code = R"(buffer = Rice::Buffer≺unsigned char≻.new([0, 127, 128, 255, 256, -128, -129, -255])
                        buffer.to_s)";
  String result = m.instance_eval(code);
  ASSERT_EQUAL("Buffer<type: unsigned char*, size: 8>", result.c_str());

  Buffer<unsigned char> buffer(nullptr);
  result = buffer.toString();
  ASSERT_EQUAL("Buffer<type: unsigned char*, size: 0>", result.c_str());
}

namespace
{
  class MyClass
  {
  public:
    MyClass(int id) : id(id)
    {
    }

    MyClass& operator=(const MyClass&) = delete;

    int id;
  };
}

TESTCASE(array_of_objects)
{
  define_buffer<MyClass*>();

  define_class<MyClass>("MyClass").
    define_constructor(Constructor<MyClass, int>()).
    define_attr("id", &MyClass::id);

  std::string code = R"(array = [MyClass.new(0), MyClass.new(1)]
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClass∗≻.new(array)
                        buffer[1].id)";

  Module m = define_module("Testing");
  Object result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));

  code = R"(array = [MyClass.new(0), MyClass.new(1)]
            buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClass∗≻.new(array)
            buffer[1] = MyClass.new(2)
            buffer[1].id)";

  result = m.module_eval(code);
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(result));
}