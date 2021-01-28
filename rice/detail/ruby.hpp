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

#include <ruby.h>

//! A function that takes a VALUE as a parameter and returns a VALUE.
// TODO: Casting from a C++ function to an extern "C" function won't
// work on all platforms.  I'm not sure what to do about this.
extern "C" typedef VALUE (*RUBY_VALUE_FUNC)(VALUE);

// Fix Ruby RUBY_METHOD_FUNC from macro to typedef
#if defined(RUBY_METHOD_FUNC)
# undef RUBY_METHOD_FUNC
  extern "C" typedef VALUE (*RUBY_METHOD_FUNC)(ANYARGS);
#endif

#ifndef RSTRING_LEN
#define RSTRING_LEN(str) RSTRING(str)->len
#endif

#ifndef RSTRING_PTR
#define RSTRING_PTR(str) RSTRING(str)->ptr
#endif

#ifndef RARRAY_LEN
#define RARRAY_LEN(arr) RARRAY(arr)->len
#endif

#ifndef RARRAY_PTR
#define RARRAY_PTR(arr) RARRAY(arr)->ptr
#endif

#ifndef RHASH_TBL
#define RHASH_TBL(hsh) RHASH(hsh)->tbl
#endif

#ifndef RCLASS_M_TBL
#define RCLASS_M_TBL(c) RCLASS(c)->m_tbl
#endif

// ruby.h has a few defines that conflict with Visual Studio's STL
#if defined(_MSC_VER)
  #undef write
  #undef read
  #undef bind
#endif

// And some c library conflicts
#undef snprintf
#undef vsnprintf

#endif // Rice__detail__ruby__hpp_

