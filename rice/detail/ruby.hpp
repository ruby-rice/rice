#ifndef Rice__detail__ruby__hpp_
#define Rice__detail__ruby__hpp_

/*! \file
 * \brief Hacks for addressing incompatibilities between various Ruby
 * versions.
 */

#include <cmath>

// Clang has to be first because on Windows it defines _MSC_VER too
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100)  // unreferenced formal parameter
#pragma warning(disable: 4702)  // unreachable code
#elif defined(__GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#include <ruby/version.h>
#include <ruby.h>
#include <ruby/encoding.h>
#include <ruby/thread.h>

// Clang has to be first because on Windows it defines _MSC_VER too
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__ )
#pragma GCC diagnostic pop
#endif

// ruby.h has a few defines that conflict with Visual Studio's STL
#if defined(_MSC_VER)
#undef write
#undef read
#undef bind
#endif

// And some c library conflicts
#undef isnan
#undef snprintf
#undef vsnprintf

//! A function that takes a VALUE as a parameter and returns a VALUE.
// TODO: Casting from a C++ function to an extern "C" function won't
// work on all platforms.  I'm not sure what to do about this.
extern "C" typedef VALUE (*RUBY_VALUE_FUNC)(VALUE);

// Fix Ruby RUBY_METHOD_FUNC from macro to typedef
#if defined(RUBY_METHOD_FUNC)
# undef RUBY_METHOD_FUNC
  extern "C" typedef VALUE (*RUBY_METHOD_FUNC)(ANYARGS);
#endif

#endif // Rice__detail__ruby__hpp_

