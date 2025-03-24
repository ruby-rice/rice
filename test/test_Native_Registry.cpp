#include "unittest.hpp"
#include "embed_ruby.hpp"

#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(NativeRegistry);

SETUP(NativeRegistry)
{
  embed_ruby();
}

TEARDOWN(NativeRegistry)
{
  rb_gc_start();
}
