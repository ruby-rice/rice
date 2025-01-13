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

TESTCASE(MakeClassName)
{
  std::string typeName = detail::typeName(typeid(Outer::Inner::Vec1));
  std::string className = detail::makeClassName(typeName);
  ASSERT_EQUAL(u8"Vector≺complex≺float≻≻", className.c_str());

  typeName = detail::typeName(typeid(Outer::Inner::Vec2));
  className = detail::makeClassName(typeName);
  ASSERT_EQUAL(u8"Vector≺unsigned char*≻", className.c_str());

  typeName = detail::typeName(typeid(Outer::Inner::Map1));
  className = detail::makeClassName(typeName);
  ASSERT_EQUAL(u8"Map≺string≺char≻‚ vector≺complex≺float≻≻≻", className.c_str());

  typeName = detail::typeName(typeid(Outer::Inner::UnorderedMap1));
  className = detail::makeClassName(typeName);
  ASSERT_EQUAL(u8"UnorderedMap≺string≺char≻‚ complex≺float≻≻", className.c_str());
}

TESTCASE(MakeRubyClass)
{
  std::string typeName = detail::typeName(typeid(Outer::Inner::Vec1));
  std::string rubyClassName = detail::makeClassName(typeName);

  Module module = define_module("Testing");

  Identifier id(rubyClassName);
  define_class_under<Outer::Inner::Vec1>(module, id);
  
  std::string code = R"(Testing.constants.grep(/Vector/).sort)";
  
  Array result = module.module_eval(code);
  ASSERT_EQUAL(1, result.size());

  // FIXME - fails compilation on GCC
  // Symbol element = result[0];
  // ASSERT_EQUAL(u8"Vector≺complex≺float≻≻", element.c_str());
}