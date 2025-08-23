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

  detail::TypeMapper<int> typeMapper;
  std::string group = typeMapper.findGroup(name, 0);
  ASSERT_EQUAL("<class cv::Vec<unsigned char, 2>, class std::allocator<class cv::Vec<unsigned char, 2> > >", group.c_str());

  group = typeMapper.findGroup(name, 18);
  ASSERT_EQUAL("<unsigned char, 2>", group.c_str());
  
  group = typeMapper.findGroup(name, 49);
  ASSERT_EQUAL("<class cv::Vec<unsigned char, 2> >", group.c_str());

  ASSERT_EXCEPTION_CHECK(
    std::runtime_error,
    typeMapper.findGroup(name, 48),
      ASSERT_EQUAL("Unbalanced Group", ex.what())
  );
}

TESTCASE(SimplifiedName)
{
  detail::TypeMapper<char*> typeMapper1;
  std::string className = typeMapper1.simplifiedName();
  ASSERT_EQUAL("char*", className.c_str());

  detail::TypeMapper<char**> typeMapper2;
  className = typeMapper2.simplifiedName();
  ASSERT_EQUAL("char**", className.c_str());

  detail::TypeMapper<double> typeMapper3;
  className = typeMapper3.simplifiedName();
  ASSERT_EQUAL("double", className.c_str());

  detail::TypeMapper<double*> typeMapper4;
  className = typeMapper4.simplifiedName();
  ASSERT_EQUAL("double*", className.c_str());

  detail::TypeMapper<std::string> typeMapper5;
  className = typeMapper5.simplifiedName();
  ASSERT_EQUAL("std::string", className.c_str());

  detail::TypeMapper<std::wstring> typeMapper6;
  className = typeMapper6.simplifiedName();
  ASSERT_EQUAL("std::wstring", className.c_str());

  detail::TypeMapper<std::vector<std::string>> typeMapper7;
  className = typeMapper7.simplifiedName();
  ASSERT_EQUAL("std::vector<std::string>", className.c_str());

  detail::TypeMapper<std::vector<std::wstring>> typeMapper8;
  className = typeMapper8.simplifiedName();
  ASSERT_EQUAL("std::vector<std::wstring>", className.c_str());

  detail::TypeMapper<std::vector<double*>> typeMapper9;
  className = typeMapper9.simplifiedName();
  ASSERT_EQUAL("std::vector<double*>", className.c_str());

  detail::TypeMapper<std::vector<double**>> typeMapper10;
  className = typeMapper10.simplifiedName();
  ASSERT_EQUAL("std::vector<double**>", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec1> typeMapper11;
  className = typeMapper11.simplifiedName();
  ASSERT_EQUAL("std::vector<std::complex<float>>", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec2> typeMapper12;
  className = typeMapper12.simplifiedName();
  ASSERT_EQUAL("std::vector<unsigned char*>", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec3> typeMapper13;
  className = typeMapper13.simplifiedName();
  ASSERT_EQUAL("std::vector<Outer::Inner::SomeClass>", className.c_str());

  detail::TypeMapper<Outer::Inner::Map1> typeMapper14;
  className = typeMapper14.simplifiedName();
  ASSERT_EQUAL("std::map<std::string, std::vector<std::complex<float>>>", className.c_str());

  detail::TypeMapper<Outer::Inner::UnorderedMap1> typeMapper15;
  className = typeMapper15.simplifiedName();
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
  ASSERT_EQUAL("Buffer≺unsigned char const∗≻", className.c_str());

  detail::TypeMapper<char**> typeMapper4;
  className = typeMapper4.rubyName();
  ASSERT_EQUAL("Buffer≺char∗∗≻", className.c_str());

  detail::TypeMapper<double> typeMapper5;
  className = typeMapper5.rubyName();
  ASSERT_EQUAL("Float", className.c_str());

  detail::TypeMapper<double*> typeMapper6;
  className = typeMapper6.rubyName();
  ASSERT_EQUAL("Buffer≺double∗≻", className.c_str());

  detail::TypeMapper<std::string> typeMapper7;
  className = typeMapper7.rubyName();
  ASSERT_EQUAL("String", className.c_str());

  detail::TypeMapper<std::wstring> typeMapper8;
  className = typeMapper8.rubyName();
  ASSERT_EQUAL("Wstring", className.c_str());

  detail::TypeMapper<std::vector<std::string>> typeMapper9;
  className = typeMapper9.rubyName();
  ASSERT_EQUAL("Vector≺string≻", className.c_str());

  detail::TypeMapper<std::vector<std::wstring>> typeMapper10;
  className = typeMapper10.rubyName();
  ASSERT_EQUAL("Vector≺wstring≻", className.c_str());

  detail::TypeMapper<std::vector<double*>> typeMapper11;
  className = typeMapper11.rubyName();
  ASSERT_EQUAL("Vector≺double∗≻", className.c_str());

  detail::TypeMapper<std::vector<double**>> typeMapper12;
  className = typeMapper12.rubyName();
  ASSERT_EQUAL("Vector≺double∗∗≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec1> typeMapper13;
  className = typeMapper13.rubyName();
  ASSERT_EQUAL("Vector≺complex≺float≻≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec2> typeMapper14;
  className = typeMapper14.rubyName();
  ASSERT_EQUAL("Vector≺unsigned char∗≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Vec3> typeMapper15;
  className = typeMapper15.rubyName();
  ASSERT_EQUAL("Vector≺Outer꞉꞉Inner꞉꞉SomeClass≻", className.c_str());

  detail::TypeMapper<Outer::Inner::Map1> typeMapper16;
  className = typeMapper16.rubyName();
  ASSERT_EQUAL("Map≺string‚ vector≺complex≺float≻≻≻", className.c_str());

  detail::TypeMapper<Outer::Inner::UnorderedMap1> typeMapper17;
  className = typeMapper17.rubyName();
  ASSERT_EQUAL("UnorderedMap≺string‚ complex≺float≻≻", className.c_str());

  detail::TypeMapper<Outer::Inner::SomeClass*> typeMapper18;
  className = typeMapper18.rubyName();
  ASSERT_EQUAL("SomeClass", className.c_str());
}

TESTCASE(RubyKlass)
{
  Module riceModule("Rice");

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
  detail::TypeMapper<unsigned char*> typeMapper7;
  actual = typeMapper7.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_buffer<char*>(); 
  expected = riceModule.const_get("Buffer≺char∗≻");
  detail::TypeMapper<char**> typeMapper8;
  actual = typeMapper8.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeMapper<double> typeMapper9;
  actual = typeMapper9.rubyKlass();
  ASSERT_EQUAL(rb_cFloat, actual);

  define_buffer<double>();
  expected = riceModule.const_get("Buffer≺double≻");
  detail::TypeMapper<double*> typeMapper10;
  actual = typeMapper10.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  detail::TypeMapper<std::string> typeMapper11;
  actual = typeMapper11.rubyKlass();
  ASSERT_EQUAL(rb_cString, actual);

  define_vector<std::string>();

  Module stdModule("Std");

  detail::TypeMapper<std::vector<std::string>> typeMapper12;
  expected = stdModule.const_get("Vector≺string≻");
  actual = typeMapper12.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::Vec1>("Vec1");
  detail::TypeMapper<Outer::Inner::Vec1> typeMapper13;
  expected = Object(rb_cObject).const_get("Vec1");
  actual = typeMapper13.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::Map1>("Map1");
  detail::TypeMapper<Outer::Inner::Map1> typeMapper14;
  expected = Object(rb_cObject).const_get("Map1");
  actual = typeMapper14.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_class<Outer::Inner::UnorderedMap1>("UnorderedMap1");
  detail::TypeMapper<Outer::Inner::UnorderedMap1> typeMapper15;
  expected = Object(rb_cObject).const_get("UnorderedMap1");
  actual = typeMapper15.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
 
  define_class<Outer::Inner::SomeClass>("SomeClass");
  define_buffer<Outer::Inner::SomeClass*>();
  detail::TypeMapper<Outer::Inner::SomeClass**> typeMapper16;
  expected = riceModule.const_get("Buffer≺Outer꞉꞉Inner꞉꞉SomeClass∗≻");
  actual = typeMapper16.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  define_buffer<void>();
  detail::TypeMapper<void*> typeMapper17;
  expected = riceModule.const_get("Buffer≺void≻");
  actual = typeMapper17.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);

  using Callback_T = char*(*)(int, double, bool, char*);
  detail::TypeMapper<Callback_T> typeMapper18;
  expected = Object(rb_cObject).const_get("Proc");
  actual = typeMapper18.rubyKlass();
  ASSERT_EQUAL(expected.value(), actual);
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