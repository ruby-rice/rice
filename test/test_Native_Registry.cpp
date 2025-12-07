#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

RICE_USE_NAMESPACE

TESTSUITE(NativeRegistry);

SETUP(NativeRegistry)
{
  embed_ruby();
}

TEARDOWN(NativeRegistry)
{
  rb_gc_start();
}
