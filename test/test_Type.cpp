#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

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

  std::string group = detail::findGroup(name, 0);
  ASSERT_EQUAL("<class cv::Vec<unsigned char, 2>, class std::allocator<class cv::Vec<unsigned char, 2> > >", group.c_str());

  group = detail::findGroup(name, 18);
  ASSERT_EQUAL("<unsigned char, 2>", group.c_str());
  
  group = detail::findGroup(name, 49);
  ASSERT_EQUAL("<class cv::Vec<unsigned char, 2> >", group.c_str());

  ASSERT_EXCEPTION_CHECK(
    std::runtime_error,
    detail::findGroup(name, 48),
      ASSERT_EQUAL("Unbalanced Group", ex.what())
  );
}

TESTCASE(CppClassName)
{
  std::string className = detail::cppClassName<char*>();
  ASSERT_EQUAL("char*", className.c_str());

  className = detail::cppClassName<char**>();
  ASSERT_EQUAL("char**", className.c_str());

  className = detail::cppClassName<double>();
  ASSERT_EQUAL("double", className.c_str());

  className = detail::cppClassName<double*>();
  ASSERT_EQUAL("double*", className.c_str());

  className = detail::cppClassName<std::string>();
  ASSERT_EQUAL("std::string", className.c_str());

  className = detail::cppClassName<std::wstring>();
  ASSERT_EQUAL("std::wstring", className.c_str());

  className = detail::cppClassName<std::vector<std::string>>();
  ASSERT_EQUAL("std::vector<std::string>", className.c_str());

  className = detail::cppClassName<std::vector<std::wstring>>();
  ASSERT_EQUAL("std::vector<std::wstring>", className.c_str());

  className = detail::cppClassName<std::vector<double*>>();
  ASSERT_EQUAL("std::vector<double*>", className.c_str());

  className = detail::cppClassName<std::vector<double**>>();
  ASSERT_EQUAL("std::vector<double**>", className.c_str());

  className = detail::cppClassName<Outer::Inner::Vec1>();
  ASSERT_EQUAL("std::vector<std::complex<float>>", className.c_str());

  className = detail::cppClassName<Outer::Inner::Vec2>();
  ASSERT_EQUAL("std::vector<unsigned char*>", className.c_str());

  className = detail::cppClassName<Outer::Inner::Vec3>();
  ASSERT_EQUAL("std::vector<Outer::Inner::SomeClass>", className.c_str());

  className = detail::cppClassName<Outer::Inner::Map1>();
  ASSERT_EQUAL("std::map<std::string, std::vector<std::complex<float>>>", className.c_str());

  className = detail::cppClassName<Outer::Inner::UnorderedMap1>();
  ASSERT_EQUAL("std::unordered_map<std::string, std::complex<float>>", className.c_str());
}

TESTCASE(RubyClassName)
{
  std::string className = detail::rubyClassName<char*>();
  ASSERT_EQUAL("String", className.c_str());

  className = detail::rubyClassName<char**>();
  ASSERT_EQUAL("Buffer≺char∗∗≻", className.c_str());

  className = detail::rubyClassName<double>();
  ASSERT_EQUAL("Float", className.c_str());

  className = detail::rubyClassName<double*>();
  ASSERT_EQUAL("Buffer≺double∗≻", className.c_str());

  className = detail::rubyClassName<std::string>();
  ASSERT_EQUAL("String", className.c_str());

  className = detail::rubyClassName<std::wstring>();
  ASSERT_EQUAL("Wstring", className.c_str());

  className = detail::rubyClassName<std::vector<std::string>>();
  ASSERT_EQUAL("Vector≺string≻", className.c_str());

  className = detail::rubyClassName<std::vector<std::wstring>>();
  ASSERT_EQUAL("Vector≺wstring≻", className.c_str());

  className = detail::rubyClassName<std::vector<double*>>();
  ASSERT_EQUAL("Vector≺double∗≻", className.c_str());

  className = detail::rubyClassName<std::vector<double**>>();
  ASSERT_EQUAL("Vector≺double∗∗≻", className.c_str());

  className = detail::rubyClassName<Outer::Inner::Vec1>();
  ASSERT_EQUAL("Vector≺complex≺float≻≻", className.c_str());

  className = detail::rubyClassName<Outer::Inner::Vec2>();
  ASSERT_EQUAL("Vector≺unsigned char∗≻", className.c_str());

  className = detail::rubyClassName<Outer::Inner::Vec3>();
  ASSERT_EQUAL("Vector≺Outer꞉꞉Inner꞉꞉SomeClass≻", className.c_str());

  className = detail::rubyClassName<Outer::Inner::Map1>();
  ASSERT_EQUAL("Map≺string‚ vector≺complex≺float≻≻≻", className.c_str());

  className = detail::rubyClassName<Outer::Inner::UnorderedMap1>();
  ASSERT_EQUAL("UnorderedMap≺string‚ complex≺float≻≻", className.c_str());

  className = detail::rubyClassName<Outer::Inner::SomeClass*>();
  ASSERT_EQUAL("SomeClass", className.c_str());
}

TESTCASE(MakeRubyClass)
{
  std::string rubyClassName = detail::rubyClassName<Outer::Inner::Vec1>();

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