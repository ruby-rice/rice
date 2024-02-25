#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <ruby/encoding.h>
#include <rice/stl.hpp>

#include <optional>

using namespace Rice;

TESTSUITE(StlStringView);

SETUP(StlStringView)
{
  embed_ruby();
}

TESTCASE(std_string_view_to_ruby)
{
  ASSERT(rb_equal(String("").value(), detail::to_ruby(std::string_view(""))));
  ASSERT(rb_equal(String("foo").value(), detail::to_ruby(std::string_view("foo"))));
}

TESTCASE(std_string_view_to_ruby_encoding)
{
  VALUE value = detail::to_ruby(std::string_view("Some String"));
  String string(value);
  Object encoding = string.call("encoding");
  String encodingName = encoding.call("name");
  std::string_view result = detail::From_Ruby<std::string_view>().convert(encodingName);
  if(result != "ASCII-8BIT" && result != "US-ASCII" && result != "UTF-8") {
    FAIL("Encoding incorrect", "ASCII-8BIT, US-ASCII, or UTF-8 (Windows)", result);
  }
}

TESTCASE(std_string_view_to_ruby_encoding_utf8)
{
  rb_encoding* defaultEncoding = rb_default_external_encoding();
  
  VALUE utf8Encoding = rb_enc_from_encoding(rb_utf8_encoding());
  rb_enc_set_default_external(utf8Encoding);

  VALUE value = detail::to_ruby(std::string_view("Some String"));
  Object object(value);
  Object encoding = object.call("encoding");
  Object encodingName = encoding.call("name");
  ASSERT_EQUAL("UTF-8", detail::From_Ruby<std::string_view>().convert(encodingName));

  rb_enc_set_default_external(rb_enc_from_encoding(defaultEncoding));
}

TESTCASE(std_string_view_from_ruby)
{
  ASSERT_EQUAL(std::string_view(""), detail::From_Ruby<std::string_view>().convert(rb_str_new2("")));
  ASSERT_EQUAL(std::string_view("foo"), detail::From_Ruby<std::string_view>().convert(rb_str_new2("foo")));

  ASSERT_EXCEPTION_CHECK(
    Exception,
    detail::From_Ruby<std::string_view>().convert(rb_float_new(15.512)),
    ASSERT_EQUAL("wrong argument type Float (expected String)", ex.what())
  );
}

TESTCASE(std_string_view_to_ruby_with_binary)
{
  Rice::String got = detail::to_ruby(std::string_view("\000test", 5));

  ASSERT_EQUAL(String(std::string_view("\000test", 5)), got);
  ASSERT_EQUAL(5ul, got.length());
}

TESTCASE(std_string_view_from_ruby_with_binary)
{
  std::string_view got = detail::From_Ruby<std::string_view>().convert(rb_str_new("\000test", 5));
  ASSERT_EQUAL(5ul, got.length());
  ASSERT_EQUAL(std::string_view("\000test", 5), got);
}

TESTCASE(std_string_view_from_ruby_refefence)
{
  VALUE value = rb_str_new("test", 4);
  std::string_view view = detail::From_Ruby<std::string_view>().convert(value);
  ASSERT_EQUAL("test", view);

  String string(value);
  string.instance_eval("self[1] = 'a'");
  //ASSERT_EQUAL("tast", view);
}
