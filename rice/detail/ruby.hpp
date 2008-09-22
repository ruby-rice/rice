#ifndef Rice__detail__ruby__hpp_
#define Rice__detail__ruby__hpp_

/*! \file
 * \brief Hacks for addressing incompatibilities between various Ruby
 * versions.
 */

#include <cmath>

// missing.h that comes with the one-click installer doesn't properly
// check for double-definition of isinf
#ifdef isinf
#define HAVE_ISINF
#endif

// TODO: Is there a way to ensure that this is Ruby's version.h?
#define RUBY_EXTERN extern "C"
#include <version.h>

// workaround for ruby 1.8.4, which defines eaccess and shouldn't
#if RUBY_VERSION_CODE <= 184
#define eaccess ruby_eaccess
#endif

#include <ruby.h>

#if RUBY_VERSION_CODE <= 184
#undef eaccess
#endif

#ifdef WIN32
#include "win32.hpp"
#endif

// This causes problems with certain C++ libraries
#undef TYPE

//! A function that takes a VALUE as a parameter and returns a VALUE.
// TODO: Casting from a C++ function to an extern "C" function won't
// work on all platforms.  I'm not sure what to do about this.
extern "C" typedef VALUE (*RUBY_VALUE_FUNC)(VALUE);

// Fix Ruby RUBY_METHOD_FUNC from macro to typedef
#if defined(RUBY_METHOD_FUNC)
# undef RUBY_METHOD_FUNC
# if RUBY_VERSION_CODE <= 166
    extern "C" typedef VALUE (*RUBY_METHOD_FUNC)();
# else
    extern "C" typedef VALUE (*RUBY_METHOD_FUNC)(ANYARGS);
# endif
#endif

// Some functions have the wrong prototype on Ruby 1.6.  Casting from a
// C++ function to an extern "C" function won't result in correct
// behavior on all platforms.  Fortunately this has been fixed in newer
// versions of Ruby.
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

