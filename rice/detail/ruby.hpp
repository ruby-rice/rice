#ifndef Rice__detail__ruby__hpp_
#define Rice__detail__ruby__hpp_

/*! \file
 * \brief Hacks for addressing incompatibilities between various Ruby
 * versions.
 */

#include <cmath>

#include <ruby.h>

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

// This is a terrible hack for Ruby 3.1 and maybe earlier to avoid crashes when test_Attribute unit cases
// are run. When the test intentionally cause exceptions to happen, the exception is correctly processed.
// However any calls back to Ruby, for example to get the exception message, crash because the ruby 
// execution context tag has been set to null. This does not happen in newer versions of Ruby.
// Should be removed when Rice no longer supports Ruby 3.1
#if RUBY_API_VERSION_MAJOR == 3 && RUBY_API_VERSION_MINOR < 2
  constexpr bool oldRuby = true;
#elif RUBY_API_VERSION_MAJOR < 3
  constexpr bool oldRuby = true;
#else
  constexpr bool oldRuby = false;
#endif 



#endif // Rice__detail__ruby__hpp_

