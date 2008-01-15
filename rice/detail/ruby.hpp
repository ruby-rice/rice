#ifndef Rice__detail__ruby__hpp_
#define Rice__detail__ruby__hpp_

/*! \file
 * \brief Hacks for addressing incompatibilities between various Ruby
 * versions.
 */

#include <ruby.h>

// TODO: Is there a way to ensure that this is Ruby's version.h?
#include <version.h>

// This causes problems with certain C++ libraries
#undef TYPE

//! A function that takes a VALUE as a parameter and returns a VALUE.
// TODO: Casting from a C++ function to an extern "C" function won't
// work on all platforms.  I'm not sure what to do about this.
extern "C" typedef VALUE (*RUBY_VALUE_FUNC)(VALUE);

// Fix Ruby RUBY_METHOD_FUNC from macro to typedef
// TODO: Casting from a C++ function to an extern "C" function won't
// result in correct behavior on all platforms.  I'm not sure what to do
// about this.
#if defined(RUBY_METHOD_FUNC)
# undef RUBY_METHOD_FUNC
# if RUBY_VERSION_CODE <= 166
    extern "C" typedef VALUE (*RUBY_METHOD_FUNC)();
# else
    extern "C" typedef VALUE (*RUBY_METHOD_FUNC)(ANYARGS);
# endif
#endif

// Some functions have the wrong prototype on Ruby 1.6
#if RUBY_VERSION_CODE < 170
  namespace Exc_Ruby
  {
    extern "C" typedef VALUE
      (*Rb_Protect_Signature)(RUBY_VALUE_FUNC, VALUE, int *);
    extern "C" typedef void
      (*Rb_Gc_Mark_Signature)(VALUE);
    extern "C" typedef void
      (*Rb_Set_End_Proc_Signature)(void (*)(VALUE), VALUE);
  }
  //! Ruby 1.6 has the wrong signature for rb_protect.
# define rb_protect(f, arg, state) \
     ((::Exc_Ruby::Rb_Protect_Signature)(rb_protect))(f, arg, state)
//! Ruby 1.6 has the wrong signature for rb_gc_mark.
# define rb_gc_mark(value) \
    ((::Exc_Ruby::Rb_Gc_Mark_Signature)(rb_gc_mark))(value)
//! Ruby 1.6 has the wrong signature for rb_set_end_proc.
# define rb_set_end_proc(f,v) \
    ((::Exc_Ruby::Rb_Set_End_Proc_Signature)(rb_set_end_proc))(f, v)
#endif

#ifndef RSTRING_LEN
#define RSTRING_LEN(str) RSTRING(str)->len
#endif

#ifndef RSTRING_PTR
#define RSTRING_PTR(str) RSTRING(str)->ptr
#endif

// ruby.h has a few defines that conflict with Visual Studio's STL
#if defined(_MSC_VER)
  #undef write
  #undef read
  #undef bind
#endif

#endif // Rice__detail__ruby__hpp_

