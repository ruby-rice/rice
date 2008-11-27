#ifndef Rice__ruby_try_catch__hpp_
#define Rice__ruby_try_catch__hpp_

#include "Exception_Base_defn.hpp"
#include "Jump_Tag.hpp"
#include "detail/ruby.hpp"
#include <stdexcept>

/*! \def RUBY_TRY
 *  \brief Start a block to catch Ruby exceptions and rethrow them.
 */
// Goto is used here to avoid having to use a second try/catch block (we
// can't rb_exc_raise directly out of the catch blocks below, since the
// exceptions will not get properly cleaned up).
// The labels are located before the try and not after it so the function can't
// "fall through" into the exception-handling code accidentally.
#define RUBY_TRY \
  VALUE Rice__ruby_exc = Qnil; \
  int Rice__ruby_jump_tag = 0; \
  \
  goto start_of_RUBY_TRY; \
  \
  Rice__ruby_exception: \
    rb_exc_raise(Rice__ruby_exc); \
  Rice__ruby_jump_tag: \
    rb_jump_tag(Rice__ruby_jump_tag); \
  \
  start_of_RUBY_TRY: \
  try

/*! \def RUBY_RETHROW(ex)
 *  \brief Given a Ruby exception as a VALUE, safely raise the exception as a
 *         Ruby exception.  This should be used inside a RUBY_TRY/RUBY_CATCH
 *         block.
 */
#define RUBY_RETHROW(ex) \
  Rice__ruby_exc = ex; \
  goto Rice__ruby_exception;

/*! \def RUBY_CATCH
 *  \brief End a RUBY_TRY block.
 */
#define RUBY_CATCH \
  catch(::Rice::Exception_Base const & ex) \
  { \
    RUBY_RETHROW(ex.value()); \
  } \
  catch(::Rice::Jump_Tag const & ex) \
  { \
    Rice__ruby_jump_tag = ex.tag; \
    goto Rice__ruby_jump_tag; \
  } \
  catch(std::bad_alloc const & ex) \
  { \
    /* This won't work quite right if the rb_exc_new2 fails; not    */ \
    /* much we can do about that, since Ruby doesn't give us access */ \
    /* to a pre-allocated NoMemoryError object                      */ \
    RUBY_RETHROW(rb_exc_new2(rb_eNoMemError, ex.what())); \
  } \
  catch(std::invalid_argument const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eArgError, ex.what())); \
  } \
  catch(std::domain_error const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eFloatDomainError, ex.what())); \
  } \
  catch(std::out_of_range const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eRangeError, ex.what())); \
  } \
  catch(std::exception const & ex) \
  { \
    /* This can raise a NoMemoryError in VERY rare circumstances    */ \
    RUBY_RETHROW(rb_exc_new2(rb_eRuntimeError, ex.what())); \
  } \
  catch(...) \
  { \
    RUBY_RETHROW(rb_exc_new2(rb_eRuntimeError, "unknown C++ exception thrown")); \
  } \

#endif // Rice__ruby_try_catch__hpp_

