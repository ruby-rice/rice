#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <complex>

using namespace Rice;

TESTSUITE(Type);

namespace Outer
{
  namespace Inner
  {
    using Vec1 = std::vector<std::complex<float>>;
    using Vec2 = std::vector<unsigned char*>;
    using Map1 = std::map<std::string, std::vector<std::complex<float>>>;
    using UnorderedMap1 = std::unordered_map<std::string, std::complex<float>>;

    class SomeClass
    {
    };

    using Vec3 = std::vector<SomeClass>;
  }
}

SETUP(Type)
{
  embed_ruby();
}

TEARDOWN(Type)
{
  rb_gc_start();
}

TESTCASE(FindGroup)
{
  std::string name = "class std::vector<class cv::Vec<unsigned char, 2>, class std::allocator<class cv::Vec<unsigned char, 2> > >";

  detail::TypeIndexParser parser(typeid(int), true);

  std::string group = parser.findGroup(name, 0);
  ASSERT_EQUAL("<class cv::Vec<unsigned char, 2>, class std::allocator<class cv::Vec<unsigned char, 2> > >", group.c_str());

  group = parser.findGroup(name, 18);
  ASSERT_EQUAL("<unsigned char, 2>", group.c_str());
  
  group = parser.findGroup(name, 49);
  ASSERT_EQUAL("<class cv::Vec<unsigned char, 2> >", group.c_str());

  ASSERT_EXCEPTION_CHECK(
    std::runtime_error,
    parser.findGroup(name, 48),
      ASSERT_EQUAL("Unbalanced Group", ex.what())
  );
}

TESTCASE(SimplifiedName)
{
  detail::TypeDetail<char*> typeDetail1;
  std::string className = typeDetail1.simplifiedName();
  ASSERT_EQUAL("char*", className.c_str());

  detail::TypeDetail<char**> typeDetail2;
  className = typeDetail2.simplifiedName();
  ASSERT_EQUAL("char**", className.c_str());

  detail::TypeDetail<double> typeDetail3;
  className = typeDetail3.simplifiedName();
  ASSERT_EQUAL("double", className.c_str());

  detail::TypeDetail<double*> typeDetail4;
  className = typeDetail4.simplifiedName();
  ASSERT_EQUAL("double*", className.c_str());

  detail::TypeDetail<std::string> typeDetail5;
  className = typeDetail5.simplifiedName();
  ASSERT_EQUAL("std::string", className.c_str());

  detail::TypeDetail<std::wstring> typeDetail6;
  className = typeDetail6.simplifiedName();
  ASSERT_EQUAL("std::wstring", className.c_str());

  detail::TypeDetail<std::vector<std::string>> typeDetail7;
  className = typeDetail7.simplifiedName();
  ASSERT_EQUAL("std::vector<std::string>", className.c_str());

  detail::TypeDetail<std::vector<std::wstring>> typeDetail8;
  className = typeDetail8.simplifiedName();
  ASSERT_EQUAL("std::vector<std::wstring>", className.c_str());

  detail::TypeDetail<std::vector<double*>> typeDetail9;
  className = typeDetail9.simplifiedName();
  ASSERT_EQUAL("std::vector<double*>", className.c_str());

  detail::TypeDetail<std::vector<double**>> typeDetail10;
  className = typeDetail10.simplifiedName();
  ASSERT_EQUAL("std::vector<double**>", className.c_str());

  detail::TypeDetail<Outer::Inner::Vec1> typeDetail11;
  className = typeDetail11.simplifiedName();
  ASSERT_EQUAL("std::vector<std::complex<float>>", className.c_str());

  detail::TypeDetail<Outer::Inner::Vec2> typeDetail12;
  className = typeDetail12.simplifiedName();
  ASSERT_EQUAL("std::vector<unsigned char*>", className.c_str());

  detail::TypeDetail<Outer::Inner::Vec3> typeDetail13;
  className = typeDetail13.simplifiedName();
  ASSERT_EQUAL("std::vector<Outer::Inner::SomeClass>", className.c_str());

  detail::TypeDetail<Outer::Inner::Map1> typeDetail14;
  className = typeDetail14.simplifiedName();
  ASSERT_EQUAL("std::map<std::string, std::vector<std::complex<float>>>", className.c_str());

  detail::TypeDetail<Outer::Inner::UnorderedMap1> typeDetail15;
  className = typeDetail15.simplifiedName();
  ASSERT_EQUAL("std::unordered_map<std::string, std::complex<float>>", className.c_str());

  detail::TypeDetail<std::function<int(int, int)>> typeDetail16;
  className = typeDetail16.simplifiedName();
  ASSERT_EQUAL("std::function<int(int, int)>", className.c_str());
}

TESTCASE(RubyName)
{
  detail::TypeDetail<char*> typeDetail1;
  std::string className = typeDetail1.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeDetail<const unsigned char> typeDetail2;
  className = typeDetail2.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeDetail<const unsigned char*> typeDetail3;
  className = typeDetail3.rubyName();
  ASSERT_EQUAL("UnsignedChar", className.c_str());

  detail::TypeDetail<char**> typeDetail4;
  className = typeDetail4.rubyName();
  ASSERT_EQUAL("Char", className.c_str());

  detail::TypeDetail<double> typeDetail5;
  className = typeDetail5.rubyName();
  ASSERT_EQUAL("Float", className.c_str());

  detail::TypeDetail<double*> typeDetail6;
  className = typeDetail6.rubyName();
  ASSERT_EQUAL("Double", className.c_str());

  detail::TypeDetail<Buffer<double*>> typeDetail7;
  className = typeDetail7.rubyName();
  ASSERT_EQUAL("Buffer≺double∗≻", className.c_str());

  detail::TypeDetail<std::string> typeDetail8;
  className = typeDetail8.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeDetail<std::wstring> typeDetail9;
  className = typeDetail9.rubyName();
  ASSERT_EQUAL("Wstring", className.c_str());

  detail::TypeDetail<std::vector<std::string>> typeDetail10;
  className = typeDetail10.rubyName();
  ASSERT_EQUAL("Vector≺string≻", className.c_str());

  detail::TypeDetail<std::vector<std::wstring>> typeDetail11;
  className = typeDetail11.rubyName();
  ASSERT_EQUAL("Vector≺wstring≻", className.c_str());

  detail::TypeDetail<std::vector<double*>> typeDetail12;
  className = typeDetail12.rubyName();
  ASSERT_EQUAL("Vector≺double∗≻", className.c_str());

  detail::TypeDetail<std::vector<double**>> typeDetail13;
  className = typeDetail13.rubyName();
  ASSERT_EQUAL("Vector≺double∗∗≻", className.c_str());

  detail::TypeDetail<Outer::Inner::Vec1> typeDetail14;
  className = typeDetail14.rubyName();
  ASSERT_EQUAL("Vector≺complex≺float≻≻", className.c_str());

  detail::TypeDetail<Outer::Inner::Vec2> typeDetail15;
  className = typeDetail15.rubyName();
  ASSERT_EQUAL("Vector≺unsigned char∗≻", className.c_str());

  detail::TypeDetail<Outer::Inner::Vec3> typeDetail16;
  className = typeDetail16.rubyName();
  ASSERT_EQUAL("Vector≺Outer꞉꞉Inner꞉꞉SomeClass≻", className.c_str());

  detail::TypeDetail<Outer::Inner::Map1> typeDetail17;
  className = typeDetail17.rubyName();
  ASSERT_EQUAL("Map≺string‚ vector≺complex≺float≻≻≻", className.c_str());

  detail::TypeDetail<Outer::Inner::UnorderedMap1> typeDetail18;
  className = typeDetail18.rubyName();
  ASSERT_EQUAL("UnorderedMap≺string‚ complex≺float≻≻", className.c_str());

  detail::TypeDetail<Outer::Inner::SomeClass*> typeDetail19;
  className = typeDetail19.rubyName();
  ASSERT_EQUAL("SomeClass", className.c_str());

  detail::TypeDetail<std::function<short*(std::string, double)>> typeDetail20;
  className = typeDetail20.rubyName();
  ASSERT_EQUAL("Function≺short∗❨string‚ double❩≻", className.c_str());
  define_stl_function<short* (std::string, double)>();
  detail::TypeDetail<std::function<short* (std::string, double)>> typeDetail31;
}

TESTCASE(RubyKlass)
{
  Module riceModule = define_module("Rice");

  detail::TypeDetail<int> typeDetail1;
  VALUE actual = typeDetail1.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeDetail<const int> typeDetail2;
  actual = typeDetail2.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeDetail<int&> typeDetail3;
  actual = typeDetail3.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeDetail<const int&> typeDetail4;
  actual = typeDetail4.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeDetail<char*> typeDetail5;
  actual = typeDetail5.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  detail::TypeDetail<const unsigned char> typeDetail6;
  actual = typeDetail6.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  define_buffer<unsigned char>();
  Object expected = riceModule.const_get("Buffer≺unsigned char≻");
  detail::TypeDetail<Buffer<unsigned char>> typeDetail8;
  actual = typeDetail8.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  expected = Object(rb_cObject).const_get("String");
  detail::TypeDetail<char*> typeDetail9;
  actual = typeDetail9.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<char*>();
  expected = riceModule.const_get("Pointer≺char∗≻");
  detail::TypeDetail<char**> typeDetail10;
  actual = typeDetail10.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<char*>();
  expected = riceModule.const_get("Pointer≺char∗≻");
  detail::TypeDetail<const char**> typeDetail11;
  actual = typeDetail11.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_buffer<char**>();
  expected = riceModule.const_get("Buffer≺char∗≻");
  detail::TypeDetail<Buffer<char*>> typeDetail12;
  actual = typeDetail12.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeDetail<double> typeDetail13;
  actual = typeDetail13.rubyKlass();
  ASSERT_EQUAL(rb_cFloat, actual);

  define_pointer<double>();
  expected = riceModule.const_get("Pointer≺double≻");
  detail::TypeDetail<volatile double*> typeDetail14;
  actual = typeDetail14.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<double*>();
  expected = riceModule.const_get("Pointer≺double∗≻");
  detail::TypeDetail<const volatile double**> typeDetail15;
  actual = typeDetail15.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeDetail<std::string> typeDetail16;
  actual = typeDetail16.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  define_pointer<std::string>();
  detail::TypeDetail<std::string*> typeDetail17;
  expected = riceModule.const_get("Pointer≺string≻");
  actual = typeDetail17.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<std::string*>();
  detail::TypeDetail<std::string**> typeDetail18;
  expected = riceModule.const_get("Pointer≺string∗≻");
  actual = typeDetail18.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
  
  define_vector<std::string>();
  Module stdModule("Std");

  detail::TypeDetail<std::vector<std::string>> typeDetail19;
  expected = stdModule.const_get("Vector≺string≻");
  actual = typeDetail19.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::Vec1>("Vec1");
  detail::TypeDetail<Outer::Inner::Vec1> typeDetail20;
  expected = Object(rb_cObject).const_get("Vec1");
  actual = typeDetail20.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::Map1>("Map1");
  detail::TypeDetail<Outer::Inner::Map1> typeDetail21;
  expected = Object(rb_cObject).const_get("Map1");
  actual = typeDetail21.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::UnorderedMap1>("UnorderedMap1");
  detail::TypeDetail<Outer::Inner::UnorderedMap1> typeDetail22;
  expected = Object(rb_cObject).const_get("UnorderedMap1");
  actual = typeDetail22.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
 
  define_class<Outer::Inner::SomeClass>("SomeClass");
  detail::TypeDetail<Outer::Inner::SomeClass**> typeDetail23;
  expected = Object(rb_cObject).const_get("SomeClass");
  actual = typeDetail23.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  using Callback_T = char*(*)(int, double, bool, char*);
  detail::TypeDetail<Callback_T> typeDetail24;
  expected = Object(rb_cObject).const_get("Proc");
  actual = typeDetail24.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeDetail<std::tuple<float, float>> typeDetail25;
  expected = Object(rb_cObject).const_get("Array");
  actual = typeDetail25.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeDetail<std::optional<float>> typeDetail26;
  actual = typeDetail26.rubyKlass();
  ASSERT_EQUAL(rb_cFloat, actual);

  detail::TypeDetail<std::complex<float>> typeDetail27;
  actual = typeDetail27.rubyKlass();
  ASSERT_EQUAL(rb_cComplex, actual);

  detail::TypeDetail<std::monostate> typeDetail28;
  actual = typeDetail28.rubyKlass();
  ASSERT_EQUAL(rb_cNilClass, actual);

  detail::TypeDetail<std::reference_wrapper<int>> typeDetail29;
  actual = typeDetail29.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeDetail<std::string_view> typeDetail30;
  actual = typeDetail30.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  define_stl_function<short*(std::string, double)>();
  detail::TypeDetail<std::function<short* (std::string, double)>> typeDetail31;
  expected = stdModule.const_get("Function≺short∗❨string‚ double❩≻");
  actual = typeDetail31.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
}

TESTCASE(MakeRubyClass)
{
  detail::TypeDetail<Outer::Inner::Vec1> typeDetail;
  std::string rubyClassName = typeDetail.rubyName();

  Module module = define_module("Testing");

  Identifier id(rubyClassName);
  define_class_under<Outer::Inner::Vec1>(module, id);
  
  std::string code = R"(Testing.constants.grep(/Vector/).sort)";
  
  Array result = module.module_eval(code);
  ASSERT_EQUAL(1, result.size());

  // FIXME - fails compilation on GCC
  // Symbol element = result[0];
  // ASSERT_EQUAL("Vector≺complex≺float≻≻", element.c_str());
}