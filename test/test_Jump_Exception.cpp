#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Jump_Tag);

SETUP(Jump_Tag)
{
}

TEARDOWN(Jump_Tag)
{
  rb_gc_start();
}

TESTCASE(construct)
{
  JumpException exception(JumpException::RUBY_TAG_RETURN);
  ASSERT_EQUAL(JumpException::RUBY_TAG_RETURN, exception.tag);
}

