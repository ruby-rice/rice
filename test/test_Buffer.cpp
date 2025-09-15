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

TESTCASE(CharMethods)
{
  define_buffer<char>();
  std::vector<detail::Native*> natives = detail::Registries::instance.natives.lookup(Data_Type<Buffer<char>>::klass());
  ASSERT_EQUAL(11, natives.size());

  define_buffer<char>();
  natives = detail::Registries::instance.natives.lookup(Data_Type<Buffer<char>>::klass());
  ASSERT_EQUAL(11, natives.size());
}

TESTCASE(CharPointerMethods)
{
  define_buffer<char*>();
  std::vector<detail::Native*> natives = detail::Registries::instance.natives.lookup(Data_Type<Buffer<char*>>::klass());
  ASSERT_EQUAL(11, natives.size());

  define_buffer<char*>();
  natives = detail::Registries::instance.natives.lookup(Data_Type<Buffer<char*>>::klass());
  ASSERT_EQUAL(11, natives.size());
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

TESTCASE(ConstChar)
{
  define_buffer<const char>();

  Module m = define_module("BufferTesting");

  std::string code = R"(Rice::Buffer≺char const≻.new("my string"))";
  Object object = m.instance_eval(code);
  ASSERT_EQUAL("Rice::Buffer≺char const≻", object.class_name().c_str());

  String bytes = object.call("bytes");
  ASSERT_EQUAL(std::string("my string"), bytes.str());

  Array array = object.call("to_ary");
  ASSERT_EQUAL("109, 121, 32, 115, 116, 114, 105, 110, 103", array.join(", ").c_str());

  Object size = object.call("size");
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(size));

  Data_Object<Buffer<const char>> dataObject(object);
  Buffer<const char> buffer = std::move(*dataObject);
  ASSERT_EQUAL(std::string("my string"), std::string(buffer.ptr(), buffer.size()));
  ASSERT_EQUAL(9, (int)buffer.size());
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

  float* values = buffer[0];
  ASSERT_IN_DELTA(1.1, values[0], 0.1);
  ASSERT_IN_DELTA(2.2, values[1], 0.1);

  values = buffer[1];
  ASSERT_IN_DELTA(3.3, values[0], 0.1);
  ASSERT_IN_DELTA(4.4, values[1], 0.1);

  values = buffer[2];
  ASSERT_IN_DELTA(5.5, values[0], 0.1);
  ASSERT_IN_DELTA(6.6, values[1], 0.1);
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
  static double Speeds[] = { 44.0, 55.0, 66.0 };
  static double* SpeedsPtr[] = { new double(44.0), new double(55.0), new double(66.0) };

  double* speeds()
  {
    return Speeds;
  }

  double** speedsPtr()
  {
    return SpeedsPtr;
  }
}

TESTCASE(pointer_of_doubles)
{
  Module m = define_module("Testing").
    define_module_function("speeds", &speeds).
    define_module_function("speeds_ptr", &speedsPtr);

  std::string code = R"(buffer = Rice::Buffer≺double≻.new(speeds, 3)
                        buffer[2])";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(66.0, detail::From_Ruby<double>().convert(result));

  code = R"(buffer_outer = Rice::Buffer≺double∗≻.new(speeds_ptr, 3)
            data = buffer_outer[2]
            buffer_inner = Rice::Buffer≺double≻.new(data, 1)
            buffer_inner[0])";

  result = m.module_eval(code);
  ASSERT_EQUAL(66.0, detail::From_Ruby<double>().convert(result));
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
                        update_pointer(buffer.data)
                        buffer.to_ary(1).first)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            update_pointer(buffer.data)
            buffer[0])";

  result = m.module_eval(code);
  ASSERT_EQUAL(5, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺int≻.new(0)
            update_pointer(buffer.data)
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

TESTCASE(update_buffer_ptr)
{
  define_buffer<int*>();

  Module m = define_module("Testing");

  std::string code = R"(buffer = Rice::Buffer≺int∗≻.new([[0, 1], [2, 3]])
                        inner = Rice::Buffer≺int≻.new(buffer[1], 2)
                        inner[1] = 9
                        inner[1])";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(result));
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
  class MyClassBuf
  {
  public:
    MyClassBuf(int id) : id(id)
    {
    }

   // MyClassBuf& operator=(const MyClassBuf&) = delete;

    int id;
  };

  static MyClassBuf myClassBufs[] = { 1,2,3 };
  static MyClassBuf* myClassBufsPtr[] = { new MyClassBuf(1), new MyClassBuf(2), new MyClassBuf(3) };

  MyClassBuf* classBufs()
  {
    return myClassBufs;
  }

  MyClassBuf** classBufsPtr()
  {
    return myClassBufsPtr;
  }

  size_t sumIds(const MyClassBuf* myClasses, size_t count)
  {
    int result = 0;
    for (int i = 0; i < count; i++)
    {
      const MyClassBuf& myClass = myClasses[i];
      result += myClass.id;
    }
    return result;
  }

  size_t sumIds(const MyClassBuf** myClasses, size_t count)
  {
    int result = 0;
    for (int i = 0; i < count; i++)
    {
      const MyClassBuf* myClass = myClasses[i];
        result += myClass->id;
    }
    return result;
  }
}

TESTCASE(array_of_objects)
{
  define_buffer<MyClassBuf>();

  define_class<MyClassBuf>("MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(array = [MyClassBuf.new(0), MyClassBuf.new(1)]
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf≻.new(array)
                        buffer[1].id)";

  Module m = define_module("Testing");
  Object result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));
}

TESTCASE(update_array_of_objects)
{
  define_buffer<MyClassBuf>();

  define_class<MyClassBuf>("MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(array = [MyClassBuf.new(0), MyClassBuf.new(1)]
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf≻.new(array)
                        buffer[1] = MyClassBuf.new(8)
                        buffer[1].id)";

  Module m = define_module("Testing");
  Object result = m.module_eval(code);
  ASSERT_EQUAL(8, detail::From_Ruby<int>().convert(result));
}

TESTCASE(pointer_of_objects)
{
  define_buffer<MyClassBuf>();
  define_buffer<MyClassBuf*>();

  Module m = define_module("Testing").
    define_module_function("class_buffs", &classBufs).
    define_module_function("class_buffs_ptr", &classBufsPtr);

  define_class<MyClassBuf>("MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf≻.new(class_buffs, 3)
                        buffer[1].id)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(2, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(class_buffs_ptr, 3)
            buffer[2].id)";

  result = m.module_eval(code);
  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(result));
}

TESTCASE(update_pointer_of_objects)
{
  define_buffer<MyClassBuf*>();

  Module m = define_module("Testing").
    define_module_function("class_buffs", &classBufs).
    define_module_function("class_buffs_ptr", &classBufsPtr);

  define_class<MyClassBuf>("MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(class_buffs_ptr, 3)
                        inner = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf≻.new(buffer[2], 1)
                        inner[0] = MyClassBuf.new(7)
                        inner[0].id)";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(7, detail::From_Ruby<int>().convert(result));

  code = R"(buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(class_buffs_ptr, 3)
            buffer[2] = MyClassBuf.new(9)
            buffer[2].id)";

  result = m.module_eval(code);
  ASSERT_EQUAL(9, detail::From_Ruby<int>().convert(result));
}

TESTCASE(array_of_const_objects)
{
  define_buffer<const MyClassBuf*>();

  define_class<MyClassBuf>("MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(array = [MyClassBuf.new(0), MyClassBuf.new(1)]
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(array)
                        buffer[1].id)";

  Module m = define_module("Testing");
  Object result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));

  code = R"(array = [MyClassBuf.new(0), MyClassBuf.new(1)]
            buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(array)
            my_class = buffer[1]
            my_class.id)";

  result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));
}

TESTCASE(array_of_objects_ptr)
{
  define_buffer<MyClassBuf*>();

  define_class<MyClassBuf>("MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(array = [MyClassBuf.new(0), MyClassBuf.new(1)]
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(array)
                        buffer[1].id)";

  Module m = define_module("Testing");
  Object result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));

  code = R"(array = [MyClassBuf.new(0), MyClassBuf.new(1)]
            buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(array)
            my_class = buffer[1]
            my_class.id)";

  result = m.module_eval(code);
  ASSERT_EQUAL(1, detail::From_Ruby<int>().convert(result));
}

TESTCASE(pass_objects)
{
  define_pointer<MyClassBuf>();
  define_buffer<MyClassBuf>();
  define_buffer<MyClassBuf*>();

  Module m = define_module("Testing");
  m.define_module_function<size_t(*)(const MyClassBuf*, size_t)>("sum_ids", &sumIds, Arg("myClasses").setArray());
  m.define_module_function<size_t(*)(const MyClassBuf**, size_t)>("sum_ids_ptr", &sumIds);

  define_class_under<MyClassBuf>(m, "MyClassBuf").
    define_constructor(Constructor<MyClassBuf, int>()).
    define_attr("id", &MyClassBuf::id);

  std::string code = R"(array = [MyClassBuf.new(5), MyClassBuf.new(7)]
                        buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf≻.new(array)
                        sum_ids(buffer.data, buffer.size))";

  Object result = m.module_eval(code);
  ASSERT_EQUAL(12, detail::From_Ruby<int>().convert(result));

  code = R"(array = [MyClassBuf.new(5), MyClassBuf.new(7)]
            buffer = Rice::Buffer≺AnonymousNamespace꞉꞉MyClassBuf∗≻.new(array)
            sum_ids_ptr(buffer.data, buffer.size))";

  result = m.module_eval(code);
  ASSERT_EQUAL(12, detail::From_Ruby<int>().convert(result));
}