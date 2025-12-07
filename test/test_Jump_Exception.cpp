#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

RICE_USE_NAMESPACE

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

