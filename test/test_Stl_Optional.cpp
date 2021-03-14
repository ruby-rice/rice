#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Optional);

SETUP(Optional)
{
  embed_ruby();
}

TESTCASE(to_ruby_has_value)
{
  std::optional<int> v = 42;
  Object o = detail::To_Ruby<std::optional<int>>::convert(v);
  ASSERT_EQUAL(INT2NUM(42), o.value());
}

TESTCASE(to_ruby_no_value)
{
  std::optional<int> v = std::nullopt;
  Object o = detail::To_Ruby<std::optional<int>>::convert(v);
  ASSERT_EQUAL(Nil, o);
}

TESTCASE(from_ruby_has_value)
{
  Object o = detail::to_ruby(42);
  std::optional<int> v = detail::From_Ruby<std::optional<int>>::convert(o);
  ASSERT(v.has_value());
  ASSERT_EQUAL(42, v.value());
}

TESTCASE(from_ruby_no_value)
{
  Object o = Nil;
  std::optional<int> v = detail::From_Ruby<std::optional<int>>::convert(o);
  ASSERT(!v.has_value());
}
