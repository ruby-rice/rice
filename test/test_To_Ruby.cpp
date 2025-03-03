#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <limits>
#include <cmath>

using namespace Rice;
using namespace std::string_literals;

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

namespace
{
  template<typename T>
  class Matrix2
  {
  public:
    T* ptr()
    {
      return this->data;
    }

    T data[5] = { 1,2,3,4,5 };
  };

  using Matrix2UnsignedChar = Matrix2<unsigned char>;
  using Matrix2Char = Matrix2<char>;
  using Matrix2UnsignedShort = Matrix2<unsigned short>;
  using Matrix2Short = Matrix2<short>;
  using Matrix2UnsignedInt = Matrix2<unsigned int>;
  using Matrix2Int = Matrix2<int>;
  using Matrix2Float = Matrix2<float>;
  using Matrix2Double = Matrix2<double>;

  template<typename T>
  class Matrix3
  {
  public:
    T** ptr()
    {
      return this->data;
    }

  private:
    T one = 1;
    T two = 2;
    T three = 3;
    T four = 4;
    T five = 5;

  public:
    T* data[5] = { &one, &two, &three, &four, &five };

  };

  using Matrix3UnsignedChar = Matrix3<unsigned char>;
}

TESTCASE(unsigned_char_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2UnsignedChar>(m, "Matrix2UnsignedChar")
    .define_constructor(Constructor<Matrix2UnsignedChar>())
    .define_method("ptr", &Matrix2UnsignedChar::ptr)
    .define_attr("data", &Matrix2UnsignedChar::data, Rice::AttrAccess::Read);

  std::string code = R"(matrix = Matrix2UnsignedChar.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";
  String buffer = m.module_eval(code);
  ASSERT_EQUAL("\x1\x2\x3\x4\x5", buffer.str());

  code = R"(matrix = Matrix2UnsignedChar.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 1))";
  buffer = m.module_eval(code);
  ASSERT_EQUAL("\x3", buffer.str());

  code = R"(matrix = Matrix2UnsignedChar.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 2))";
  buffer = m.module_eval(code);
  ASSERT_EQUAL("\x3\x4", buffer.str());

  code = R"(matrix = Matrix2UnsignedChar.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 0))";
  buffer = m.module_eval(code);
  ASSERT_EQUAL("", buffer.str());

  code = R"(matrix = Matrix2UnsignedChar.new
            pointer_view = matrix.data
            pointer_view.read)";

  buffer = m.module_eval(code);
  ASSERT_EQUAL("\x1\x2\x3\x4\x5", buffer.str());
}

TESTCASE(unsigned_char_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2UnsignedChar>(m, "Matrix2UnsignedChar")
    .define_constructor(Constructor<Matrix2UnsignedChar>())
    .define_method("ptr", &Matrix2UnsignedChar::ptr);

  std::string code = R"(matrix = Matrix2UnsignedChar.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

  std::vector<unsigned char> expected = std::vector<unsigned char>{ 1,2,3,4,5 };
  Array array = m.module_eval(code);
  std::vector<unsigned char> actual = array.to_vector<unsigned char>();
  ASSERT_EQUAL(expected, actual);

  code = R"(matrix = Matrix2UnsignedChar.new
           pointer_view = matrix.ptr
           pointer_view.to_a(0, 1))";

  expected = std::vector<unsigned char>{ 1 };
  array = m.module_eval(code);
  actual = array.to_vector<unsigned char>();
  ASSERT_EQUAL(expected, actual);
}

TESTCASE(unsigned_char_ptr_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix3UnsignedChar>(m, "Matrix3UnsignedChar")
    .define_constructor(Constructor<Matrix3UnsignedChar>())
    .define_method("ptr", &Matrix3UnsignedChar::ptr)
    .define_attr("data", &Matrix3UnsignedChar::data, Rice::AttrAccess::Read);

  std::string code = R"(matrix = Matrix3UnsignedChar.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";
  String buffer = m.module_eval(code);
  ASSERT_EQUAL(5, buffer.length());
}

TESTCASE(unsigned_char_ptr_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix3UnsignedChar>(m, "Matrix3UnsignedChar")
    .define_constructor(Constructor<Matrix3UnsignedChar>())
    .define_method("ptr", &Matrix3UnsignedChar::ptr);

  std::string code = R"(matrix = Matrix3UnsignedChar.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

  Array pointers = m.module_eval(code);
  ASSERT_EQUAL(5, pointers.size());

  for (int i = 0; i < pointers.size(); i++)
  {
    Object pointerView = pointers[i];
    Array array = pointerView.call("to_a", 0, 1);

    std::vector<unsigned char> expected = std::vector<unsigned char>{ (unsigned char)(i + 1) };
    std::vector<unsigned char> actual = array.to_vector<unsigned char>();
    ASSERT_EQUAL(expected, actual);
  }
}

TESTCASE(short_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Short>(m, "Matrix2Short")
    .define_constructor(Constructor<Matrix2Short>())
    .define_method("ptr", &Matrix2Short::ptr);

  std::string code = R"(matrix = Matrix2Short.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";

  std::string expected = "\x1\0\x2\0\x3\0\x4\0\x5\0"s;
  String buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Short.new
                        pointer_view = matrix.ptr
                        pointer_view.read(2, 1))";
  expected = "\x3\0"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Short.new
                        pointer_view = matrix.ptr
                        pointer_view.read(2, 2))";

  expected = "\x3\0\x4\0"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Short.new
                        pointer_view = matrix.ptr
                        pointer_view.read(2, 0))";
  expected = ""s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());
}

TESTCASE(short_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Short>(m, "Matrix2Short")
    .define_constructor(Constructor<Matrix2Short>())
    .define_method("ptr", &Matrix2Short::ptr);

  std::string code = R"(matrix = Matrix2Short.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

   std::vector<short> expected = std::vector<short>{1,2,3,4,5};
   Array array = m.module_eval(code);
   std::vector<short> actual = array.to_vector<short>();
   ASSERT_EQUAL(expected, actual);

   code = R"(matrix = Matrix2Short.new
             pointer_view = matrix.ptr
             pointer_view.to_a(3, 2))";

    expected = std::vector<short>{4, 5};
    array = m.module_eval(code);
    actual = array.to_vector<short>();
    ASSERT_EQUAL(expected, actual);
}

TESTCASE(unsigned_short_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2UnsignedShort>(m, "Matrix2UnsignedShort")
    .define_constructor(Constructor<Matrix2UnsignedShort>())
    .define_method("ptr", &Matrix2UnsignedShort::ptr);

  std::string code = R"(matrix = Matrix2UnsignedShort.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";

  std::string expected = "\x1\0\x2\0\x3\0\x4\0\x5\0"s;
  String buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2UnsignedShort.new
                        pointer_view = matrix.ptr
                        pointer_view.read(2, 1))";
  expected = "\x3\0"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2UnsignedShort.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 2))";

  expected = "\x3\0\x4\0"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2UnsignedShort.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 0))";
  expected = ""s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());
}

TESTCASE(unsigned_short_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2UnsignedShort>(m, "Matrix2UnsignedShort")
    .define_constructor(Constructor<Matrix2UnsignedShort>())
    .define_method("ptr", &Matrix2UnsignedShort::ptr);

  std::string code = R"(matrix = Matrix2UnsignedShort.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

  std::vector<unsigned short> expected = std::vector<unsigned short>{ 1,2,3,4,5 };
  Array array = m.module_eval(code);
  std::vector<unsigned short> actual = array.to_vector<unsigned short>();
  ASSERT_EQUAL(expected, actual);

  code = R"(matrix = Matrix2UnsignedShort.new
            pointer_view = matrix.ptr
            pointer_view.to_a(3, 2))";

  expected = std::vector<unsigned short>{ 4, 5 };
  array = m.module_eval(code);
  actual = array.to_vector<unsigned short>();
  ASSERT_EQUAL(expected, actual);
}

TESTCASE(int_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Int>(m, "Matrix2Int")
    .define_constructor(Constructor<Matrix2Int>())
    .define_method("ptr", &Matrix2Int::ptr);

  std::string code = R"(matrix = Matrix2Int.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";

  std::string expected = "\x1\0\0\0\x2\0\0\0\x3\0\0\0\x4\0\0\0\x5\0\0\0"s;
  String buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Int.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 1))";
  expected = "\x3\0\0\0"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Int.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 2))";

  expected = "\x3\0\0\0\x4\0\0\0"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Int.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 0))";
  expected = ""s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());
}

TESTCASE(int_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Int>(m, "Matrix2Int")
    .define_constructor(Constructor<Matrix2Int>())
    .define_method("ptr", &Matrix2Int::ptr);

  std::string code = R"(matrix = Matrix2Int.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

  std::vector<int> expected = std::vector<int>{ 1,2,3,4,5 };
  Array array = m.module_eval(code);
  std::vector<int> actual = array.to_vector<int>();
  ASSERT_EQUAL(expected, actual);

  code = R"(matrix = Matrix2Int.new
            pointer_view = matrix.ptr
            pointer_view.to_a(3, 2))";

  expected = std::vector<int>{ 4, 5 };
  array = m.module_eval(code);
  actual = array.to_vector<int>();
  ASSERT_EQUAL(expected, actual);
}

TESTCASE(float_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Float>(m, "Matrix2Float")
    .define_constructor(Constructor<Matrix2Float>())
    .define_method("ptr", &Matrix2Float::ptr);

  std::string code = R"(matrix = Matrix2Float.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";

  std::string expected = "\0\0\x80\x3f\0\0\0\x40\0\0\x40\x40\0\0\x80\x40\0\0\xa0\x40"s;
  String buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Float.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 1))";
  expected = "\0\0\x40\x40"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Float.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 2))";

  expected = "\0\0\x40\x40\0\0\x80\x40"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Float.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 0))";
  expected = ""s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());
}

TESTCASE(float_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Float>(m, "Matrix2Float")
    .define_constructor(Constructor<Matrix2Float>())
    .define_method("ptr", &Matrix2Float::ptr);

  std::string code = R"(matrix = Matrix2Float.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

  std::vector<float> expected = std::vector<float>{ 1.0,2.0,3.0,4.0,5.0 };
  Array array = m.module_eval(code);
  std::vector<float> actual = array.to_vector<float>();
  ASSERT_EQUAL(expected, actual);

  code = R"(matrix = Matrix2Float.new
             pointer_view = matrix.ptr
             pointer_view.to_a(3, 2))";

  expected = std::vector<float>{ 4.0, 5.0 };
  array = m.module_eval(code);
  actual = array.to_vector<float>();
  ASSERT_EQUAL(expected, actual);
}

TESTCASE(double_ptr_buffer)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Double>(m, "Matrix2Double")
    .define_constructor(Constructor<Matrix2Double>())
    .define_method("ptr", &Matrix2Double::ptr);

  std::string code = R"(matrix = Matrix2Double.new
                        pointer_view = matrix.ptr
                        pointer_view.read(0, 5))";

  std::string expected = "\0\0\0\0\0\0\xf0\x3f\0\0\0\0\0\0\0\x40\0\0\0\0\0\0\x08\x40\0\0\0\0\0\0\x10\x40\0\0\0\0\0\0\x14\x40"s;
  String buffer = m.module_eval(code);
  const char* ptr = RSTRING_PTR(buffer.value());
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Double.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 1))";
  expected = "\0\0\0\0\0\0\x08\x40"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Double.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 2))";

  expected = "\0\0\0\0\0\0\x08\x40\0\0\0\0\0\0\x10\x40"s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());

  code = R"(matrix = Matrix2Double.new
            pointer_view = matrix.ptr
            pointer_view.read(2, 0))";
  expected = ""s;
  buffer = m.module_eval(code);
  ASSERT_EQUAL(expected, buffer.str());
}

TESTCASE(double_ptr_array)
{
  Module m = define_module("ToRubyPtr");

  Class c = define_class_under<Matrix2Double>(m, "Matrix2Double")
    .define_constructor(Constructor<Matrix2Double>())
    .define_method("ptr", &Matrix2Double::ptr);

  std::string code = R"(matrix = Matrix2Double.new
                        pointer_view = matrix.ptr
                        pointer_view.to_a(0, 5))";

  std::vector<double> expected = std::vector<double>{ 1.0,2.0,3.0,4.0,5.0 };
  Array array = m.module_eval(code);
  std::vector<double> actual = array.to_vector<double>();
  ASSERT_EQUAL(expected, actual);

  code = R"(matrix = Matrix2Double.new
             pointer_view = matrix.ptr
             pointer_view.to_a(3, 2))";

  expected = std::vector<double>{ 4.0, 5.0 };
  array = m.module_eval(code);
  actual = array.to_vector<double>();
  ASSERT_EQUAL(expected, actual);
}
