#include "protect.hpp"
#include "../Exception.hpp"
#include "../Jump_Tag.hpp"

#ifndef TAG_RAISE
#define TAG_RAISE 0x6
#endif

VALUE Rice::detail::
protect(
    RUBY_VALUE_FUNC f,
    VALUE arg)
{
  int state = 0;
  VALUE retval = rb_protect(f, arg, &state);
  if(state != 0)
  {
    VALUE err = rb_errinfo();
    if(state == TAG_RAISE && RTEST(err))
    {
      // TODO: convert NoMemoryError into bad_alloc?
      rb_set_errinfo(Rice::Nil);
      throw Rice::Exception(err);
    }
    throw Jump_Tag(state);
  }
  return retval;
}

