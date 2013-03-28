#ifndef Exc_Ruby___cpp__st__hpp_
#define Exc_Ruby___cpp__st__hpp_

/*! \file
 * \brief Hacks to allow functions in st.h to be called from C++
 * programs.
 */

#include "ruby.hpp"

// Ruby doesn't put extern "C" around st.h

extern "C"
{
#ifdef RUBY_VM
#include "ruby/st.h"
#else
#include "st.h"
#endif
}

#endif // Exc_Ruby___cpp__st__hpp_
