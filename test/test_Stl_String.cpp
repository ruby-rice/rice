#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <optional>

using namespace Rice;

TESTSUITE(StlString);

SETUP(StlString)
{
  embed_ruby();
}

TESTCASE(std_string_to_ruby)
{
  ASSERT(rb_equal(String("").value(), detail::to_ruby(std::string(""))));
  ASSERT(rb_equal(String("foo").value(), detail::to_ruby(std::string("foo"))));
}

TESTCASE(std_string_from_ruby)
{
  ASSERT_EQUAL(std::string(""), detail::From_Ruby<std::string>::convert(rb_str_new2("")));
  ASSERT_EQUAL(std::string("foo"), detail::From_Ruby<std::string>::convert(rb_str_new2("foo")));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<std::string> ::convert(rb_float_new(15.512)),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}

TESTCASE(std_string_to_ruby_with_binary)
{
  Rice::String got = detail::to_ruby(std::string("\000test", 5));

  ASSERT_EQUAL(String(std::string("\000test", 5)), got);
  ASSERT_EQUAL(5ul, got.length());
}

TESTCASE(std_string_from_ruby_with_binary)
{
  std::string got = detail::From_Ruby<std::string>::convert(rb_str_new("\000test", 5));
  ASSERT_EQUAL(5ul, got.length());
  ASSERT_EQUAL(std::string("\000test", 5), got);
}