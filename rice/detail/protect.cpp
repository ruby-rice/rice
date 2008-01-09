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
    if(state == TAG_RAISE && ruby_errinfo != Qnil)
    {
      // TODO: convert NoMemoryError into bad_alloc?
      throw Rice::Exception(ruby_errinfo);
    }
    throw Jump_Tag(state);
  }
  return retval;
}

