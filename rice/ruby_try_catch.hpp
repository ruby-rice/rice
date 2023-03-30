#ifndef Rice__ruby_try_catch__hpp_
#define Rice__ruby_try_catch__hpp_

#include <stdexcept>

#include "detail/Jump_Tag.hpp"
#include "Exception_defn.hpp"

template <typename Callable_T>
auto cpp_protect(Callable_T && func)
{
  try
  {
    return func();
  }
  catch (::Rice::Exception const& ex)
  {
    rb_exc_raise(ex.value());
  }
  catch (::Rice::Jump_Tag const& ex)
  {
    rb_jump_tag(ex.tag);
  }
  catch (std::bad_alloc const& ex)
  {
    /* This won't work quite right if the rb_exc_new2 fails; not
       much we can do about that, since Ruby doesn't give us access
       to a pre-allocated NoMemoryError object */
    rb_exc_raise(rb_exc_new2(rb_eNoMemError, ex.what()));
  }
  catch (std::invalid_argument const& ex)
  {
    // This can raise a NoMemoryError in VERY rare circumstances
    rb_exc_raise(rb_exc_new2(rb_eArgError, ex.what()));
  }
  catch (std::domain_error const& ex)
  {
    // This can raise a NoMemoryError in VERY rare circumstances
    rb_exc_raise(rb_exc_new2(rb_eFloatDomainError, ex.what()));
  }
  catch (std::out_of_range const& ex)
  {
    // This can raise a NoMemoryError in VERY rare circumstances
    rb_exc_raise(rb_exc_new2(rb_eRangeError, ex.what()));
  }
  catch (std::exception const& ex)
  {
    // This can raise a NoMemoryError in VERY rare circumstances
    rb_exc_raise(rb_exc_new2(rb_eRuntimeError, ex.what()));
  }
  catch (...)
  {
    rb_exc_raise(rb_exc_new2(rb_eRuntimeError, "unknown C++ exception thrown"));
  }
}
#endif // Rice__ruby_try_catch__hpp_

