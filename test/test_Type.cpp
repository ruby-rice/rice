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
  detail::TypeIndexParser typeIndexParser1(typeid(char*));
  std::string className = typeIndexParser1.simplifiedName();
  ASSERT_EQUAL("char*", className.c_str());

  detail::TypeIndexParser typeIndexParser2(typeid(char**));
  className = typeIndexParser2.simplifiedName();
  ASSERT_EQUAL("char**", className.c_str());

  detail::TypeIndexParser typeIndexParser3(typeid(double));
  className = typeIndexParser3.simplifiedName();
  ASSERT_EQUAL("double", className.c_str());

  detail::TypeIndexParser typeIndexParser4(typeid(double*));
  className = typeIndexParser4.simplifiedName();
  ASSERT_EQUAL("double*", className.c_str());

  detail::TypeIndexParser typeIndexParser5(typeid(std::string));
  className = typeIndexParser5.simplifiedName();
  ASSERT_EQUAL("std::string", className.c_str());

  detail::TypeIndexParser typeIndexParser6(typeid(std::wstring));
  className = typeIndexParser6.simplifiedName();
  ASSERT_EQUAL("std::wstring", className.c_str());

  detail::TypeIndexParser typeIndexParser7(typeid(std::vector<std::string>));
  className = typeIndexParser7.simplifiedName();
  ASSERT_EQUAL("std::vector<std::string>", className.c_str());

  detail::TypeIndexParser typeIndexParser8(typeid(std::vector<std::wstring>));
  className = typeIndexParser8.simplifiedName();
  ASSERT_EQUAL("std::vector<std::wstring>", className.c_str());

  detail::TypeIndexParser typeIndexParser9(typeid(std::vector<double*>));
  className = typeIndexParser9.simplifiedName();
  ASSERT_EQUAL("std::vector<double*>", className.c_str());

  detail::TypeIndexParser typeIndexParser10(typeid(std::vector<double**>));
  className = typeIndexParser10.simplifiedName();
  ASSERT_EQUAL("std::vector<double**>", className.c_str());

  detail::TypeIndexParser typeIndexParser11(typeid(Outer::Inner::Vec1));
  className = typeIndexParser11.simplifiedName();
  ASSERT_EQUAL("std::vector<std::complex<float>>", className.c_str());

  detail::TypeIndexParser typeIndexParser12(typeid(Outer::Inner::Vec2));
  className = typeIndexParser12.simplifiedName();
  ASSERT_EQUAL("std::vector<unsigned char*>", className.c_str());

  detail::TypeIndexParser typeIndexParser13(typeid(Outer::Inner::Vec3));
  className = typeIndexParser13.simplifiedName();
  ASSERT_EQUAL("std::vector<Outer::Inner::SomeClass>", className.c_str());

  detail::TypeIndexParser typeIndexParser14(typeid(Outer::Inner::Map1));
  className = typeIndexParser14.simplifiedName();
  ASSERT_EQUAL("std::map<std::string, std::vector<std::complex<float>>>", className.c_str());

  detail::TypeIndexParser typeIndexParser15(typeid(Outer::Inner::UnorderedMap1));
  className = typeIndexParser15.simplifiedName();
  ASSERT_EQUAL("std::unordered_map<std::string, std::complex<float>>", className.c_str());
}

TESTCASE(RubyName)
{
  detail::TypeMapper<char*> typeMapper1;
  std::string className = typeMapper1.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeMapper<const unsigned char> typeMapper2;
  className = typeMapper2.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeMapper<const unsigned char*> typeMapper3;
  className = typeMapper3.rubyName();
  ASSERT_EQUAL("UnsignedChar", className.c_str());

  detail::TypeMapper<char**> typeMapper4;
  className = typeMapper4.rubyName();
  ASSERT_EQUAL("Char", className.c_str());

  detail::TypeMapper<double> typeMapper5;
  className = typeMapper5.rubyName();
  ASSERT_EQUAL("Float", className.c_str());

  detail::TypeMapper<double*> typeMapper6;
  className = typeMapper6.rubyName();
  ASSERT_EQUAL("Double", className.c_str());

  detail::TypeMapper<Buffer<double*>> typeMapper7;
  className = typeMapper7.rubyName();
  ASSERT_EQUAL("Buffer≺double∗≻", className.c_str());

  detail::TypeMapper<std::string> typeMapper8;
  className = typeMapper8.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeMapper<std::wstring> typeMapper9;
  className = typeMapper9.rubyName();
  ASSERT_EQUAL("Wstring", className.c_str());

  detail::TypeMapper<std::vector<std::string>> typeMapper10;
  className = typeMapper10.rubyName();
  ASSERT_EQUAL("Vector≺string≻", className.c_str());

  detail::TypeMapper<std::vector<std::wstring>> typeMapper11;
  className = typeMapper11.rubyName();
  ASSERT_EQUAL("Vector≺wstring≻", className.c_str());

  detail::TypeMapper<std::vector<double*>> typeMapper12;
  className = typeMapper12.rubyName();
  ASSERT_EQUAL("Vector≺double∗≻", className.c_str());

  detail::TypeMapper<std::vector<double**>> typeMapper13;
  className = typeMapper13.rubyName();
  ASSERT_EQUAL("Vector≺double∗∗≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec1> typeMapper14;
  className = typeMapper14.rubyName();
  ASSERT_EQUAL("Vector≺complex≺float≻≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec2> typeMapper15;
  className = typeMapper15.rubyName();
  ASSERT_EQUAL("Vector≺unsigned char∗≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec3> typeMapper16;
  className = typeMapper16.rubyName();
  ASSERT_EQUAL("Vector≺Outer꞉꞉Inner꞉꞉SomeClass≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Map1> typeMapper17;
  className = typeMapper17.rubyName();
  ASSERT_EQUAL("Map≺string‚ vector≺complex≺float≻≻≻", className.c_str());

  detail::TypeMapper<Outer::Inner::UnorderedMap1> typeMapper18;
  className = typeMapper18.rubyName();
  ASSERT_EQUAL("UnorderedMap≺string‚ complex≺float≻≻", className.c_str());

  detail::TypeMapper<Outer::Inner::SomeClass*> typeMapper19;
  className = typeMapper19.rubyName();
  ASSERT_EQUAL("SomeClass", className.c_str());
}

TESTCASE(RubyKlass)
{
  Module riceModule = define_module("Rice");

  detail::TypeMapper<int> typeMapper1;
  VALUE actual = typeMapper1.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeMapper<const int> typeMapper2;
  actual = typeMapper2.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeMapper<int&> typeMapper3;
  actual = typeMapper3.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeMapper<const int&> typeMapper4;
  actual = typeMapper4.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeMapper<char*> typeMapper5;
  actual = typeMapper5.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  detail::TypeMapper<const unsigned char> typeMapper6;
  actual = typeMapper6.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  define_buffer<unsigned char>();
  Object expected = riceModule.const_get("Buffer≺unsigned char≻");
  detail::TypeMapper<Buffer<unsigned char>> typeMapper8;
  actual = typeMapper8.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  expected = Object(rb_cObject).const_get("String");
  detail::TypeMapper<char*> typeMapper9;
  actual = typeMapper9.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<char*>();
  expected = riceModule.const_get("Pointer≺char∗≻");
  detail::TypeMapper<char**> typeMapper10;
  actual = typeMapper10.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<char*>();
  expected = riceModule.const_get("Pointer≺char∗≻");
  detail::TypeMapper<const char**> typeMapper11;
  actual = typeMapper11.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_buffer<char**>();
  expected = riceModule.const_get("Buffer≺char∗≻");
  detail::TypeMapper<Buffer<char*>> typeMapper12;
  actual = typeMapper12.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeMapper<double> typeMapper13;
  actual = typeMapper13.rubyKlass();
  ASSERT_EQUAL(rb_cFloat, actual);

  define_pointer<double>();
  expected = riceModule.const_get("Pointer≺double≻");
  detail::TypeMapper<volatile double*> typeMapper14;
  actual = typeMapper14.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<double*>();
  expected = riceModule.const_get("Pointer≺double∗≻");
  detail::TypeMapper<const volatile double**> typeMapper15;
  actual = typeMapper15.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeMapper<std::string> typeMapper16;
  actual = typeMapper16.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  define_pointer<std::string>();
  detail::TypeMapper<std::string*> typeMapper17;
  expected = riceModule.const_get("Pointer≺string≻");
  actual = typeMapper17.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_pointer<std::string*>();
  detail::TypeMapper<std::string**> typeMapper18;
  expected = riceModule.const_get("Pointer≺string∗≻");
  actual = typeMapper18.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
  
  define_vector<std::string>();
  Module stdModule("Std");

  detail::TypeMapper<std::vector<std::string>> typeMapper19;
  expected = stdModule.const_get("Vector≺string≻");
  actual = typeMapper19.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::Vec1>("Vec1");
  detail::TypeMapper<Outer::Inner::Vec1> typeMapper20;
  expected = Object(rb_cObject).const_get("Vec1");
  actual = typeMapper20.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::Map1>("Map1");
  detail::TypeMapper<Outer::Inner::Map1> typeMapper21;
  expected = Object(rb_cObject).const_get("Map1");
  actual = typeMapper21.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::UnorderedMap1>("UnorderedMap1");
  detail::TypeMapper<Outer::Inner::UnorderedMap1> typeMapper22;
  expected = Object(rb_cObject).const_get("UnorderedMap1");
  actual = typeMapper22.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
 
  define_class<Outer::Inner::SomeClass>("SomeClass");
  detail::TypeMapper<Outer::Inner::SomeClass**> typeMapper23;
  expected = Object(rb_cObject).const_get("SomeClass");
  actual = typeMapper23.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  using Callback_T = char*(*)(int, double, bool, char*);
  detail::TypeMapper<Callback_T> typeMapper24;
  expected = Object(rb_cObject).const_get("Proc");
  actual = typeMapper24.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeMapper<std::tuple<float, float>> typeMapper25;
  expected = Object(rb_cObject).const_get("Array");
  actual = typeMapper25.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeMapper<std::optional<float>> typeMapper26;
  actual = typeMapper26.rubyKlass();
  ASSERT_EQUAL(rb_cFloat, actual);

  detail::TypeMapper<std::complex<float>> typeMapper27;
  actual = typeMapper27.rubyKlass();
  ASSERT_EQUAL(rb_cComplex, actual);

  detail::TypeMapper<std::monostate> typeMapper28;
  actual = typeMapper28.rubyKlass();
  ASSERT_EQUAL(rb_cNilClass, actual);

  detail::TypeMapper<std::reference_wrapper<int>> typeMapper29;
  actual = typeMapper29.rubyKlass();
  ASSERT_EQUAL(rb_cInteger, actual);

  detail::TypeMapper<std::string_view> typeMapper30;
  actual = typeMapper30.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);
}

TESTCASE(MakeRubyClass)
{
  detail::TypeMapper<Outer::Inner::Vec1> typeMapper;
  std::string rubyClassName = typeMapper.rubyName();

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