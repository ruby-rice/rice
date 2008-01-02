#include "protect.hpp"
#include "../Exception.hpp"
#include "../Jump_Tag.hpp"

VALUE Rice::detail::
protect(
    RUBY_VALUE_FUNC f,
    VALUE arg)
{
  int state = 0;
  VALUE retval = rb_protect(f, arg, &state);
  if(state != 0)
  {
    if(state == 6 && ruby_errinfo != Qnil) // TAG_RAISE == 6
    {
      // TODO: convert NoMemoryError into bad_alloc?
      throw Rice::Exception(ruby_errinfo);
    }
    throw Jump_Tag(state);
  }
  return retval;
}

