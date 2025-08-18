#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Gvl);

SETUP(Gvl)
{
  embed_ruby();
}

TEARDOWN(Gvl)
{
  rb_gc_start();
}

TESTCASE(with_and_without_gvl_void)
{
  int result = 0;
  detail::without_gvl([&] {
    ++result;
    detail::with_gvl([&] { ++result; });
  });
  ASSERT_EQUAL(result, 2);
}

TESTCASE(with_and_without_gvl_return)
{
  int result = detail::without_gvl([] {
    return 1 + detail::with_gvl([] { return 1; });
  });
  ASSERT_EQUAL(result, 2);
}

TESTCASE(without_gvl_args)
{
  int result = detail::without_gvl(RUBY_UBF_PROCESS, nullptr, RB_NOGVL_INTR_FAIL, [] {
    return 1;
  });
  ASSERT_EQUAL(result, 1);
}
