#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>
#include <rice/stl.hpp>

using namespace Rice;

TESTSUITE(Vector);

SETUP(Vector)
{
  embed_ruby();
}

TESTCASE(to_ruby)
{
  std::vector<int> v = {42, 43, 44};
  Array a = detail::To_Ruby<std::vector<int>>::convert(v);
  ASSERT_EQUAL(3u, a.size());
  ASSERT_EQUAL(INT2NUM(42), a[0].value());
  ASSERT_EQUAL(INT2NUM(43), a[1].value());
  ASSERT_EQUAL(INT2NUM(44), a[2].value());
}

TESTCASE(from_ruby)
{
  Array a;
  a.push(42);
  a.push(43);
  a.push(44);
  std::vector<int> v = detail::From_Ruby<std::vector<int>>::convert(a);
  ASSERT_EQUAL(3u, v.size());
  ASSERT_EQUAL(42, v[0]);
  ASSERT_EQUAL(43, v[1]);
  ASSERT_EQUAL(44, v[2]);
}
