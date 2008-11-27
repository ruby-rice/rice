#include "unittest.hpp"
#include "rice/Critical_Guard.hpp"
#include <stdexcept>

#if RICE__RUBY_VERSION_CODE < 190

using namespace Rice;

TESTSUITE(Critical_Guard);

SETUP(Critical_Guard)
{
  ruby_init();
}

TESTCASE(normal_path)
{
  rb_thread_critical = 1;
  {
    Critical_Guard g;
    ASSERT(rb_thread_critical);
  }
  ASSERT(!rb_thread_critical);
}

TESTCASE(unset_thread_critical_on_destruction)
{
  rb_thread_critical = 1;
  {
    Critical_Guard g;
    ASSERT(rb_thread_critical);
  }
  ASSERT(!rb_thread_critical);
}

TESTCASE(exception)
{
  rb_thread_critical = 1;
  try
  {
    Critical_Guard g;
    throw std::runtime_error("testing");
  }
  catch(...)
  {
  }
  ASSERT(!rb_thread_critical);
}

#endif

