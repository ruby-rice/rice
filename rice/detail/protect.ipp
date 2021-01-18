#include "ruby.hpp"
#include "Jump_Tag.hpp"
// TODO - don't like pulling in Rice headers
#include "../Exception.hpp"

namespace Rice
{
  namespace detail
  {
    const int TAG_RAISE = 0x6;

    inline VALUE Rice::detail::
    protect(
        RUBY_VALUE_FUNC f,
        VALUE arg)
    {
      int state = 0;
      VALUE retval = rb_protect(f, arg, &state);
      if (state != 0)
      {
        VALUE err = rb_errinfo();
        if (state == TAG_RAISE && RTEST(err))
        {
          // TODO: convert NoMemoryError into bad_alloc?
          rb_set_errinfo(Qnil);
          throw Rice::Exception(err);
        }
        else
        {
          throw Jump_Tag(state);
        }
      }
      return retval;
    }
  }
}