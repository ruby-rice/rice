#ifndef Rice__detail__protect__hpp_
#define Rice__detail__protect__hpp_

#include "ruby.hpp"

/*! \file
 *  \brief Functions for making exception-safe calls into Ruby code.
 *  These are the building blocks for building other exception-safe
 *  helper functions.
 */

namespace Rice
{

namespace detail
{

//! Call the given function, converting Ruby exceptions to C++
//! exceptions.
/*! Call the function f with the parameter arg  If f raises a Ruby
 * exception, then the exception is re-thrown as an Exception.  If f
 * exits with any other non-zero tag (e.g. a Symbol is thrown), then the
 * tag is re-thrown as a Jump_Tag.
 */
VALUE protect(
    RUBY_VALUE_FUNC f,
    VALUE arg);

} // namespace detail

} // namespace Rice

#endif // Rice__detail__protect__hpp_

