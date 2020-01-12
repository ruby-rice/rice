#ifndef Rice__protect__hpp_
#define Rice__protect__hpp_

// This causes problems with certain C++ libraries
#undef TYPE

#include "Object_defn.hpp"

namespace Rice
{

/*! \file
 *  \brief A collection of functions (overloaded on number of
 *  arguments) for calling C functions that might raise Ruby exceptions.
 */

//! Call the C function f with arguments (arg1, arg2, ...).
/*! E.g.:
 *  \code
 *    VALUE x = protect(rb_ary_new);
 *    protect(rb_ary_push, x, INT2NUM(42));
 *  \endcode
 *
 *  Note that this function makes copies of all of its arguments; it
 *  does not take anything by reference.  All of the copies are const so
 *  that protect will not work if f takes a non-const
 *  reference to any of its arguments (though you can use non-const
 *  pointers).
 */
template<typename Fun, typename ...ArgT>
VALUE protect(Fun fun, ArgT const &... args);

} // namespace Rice

#include "protect.ipp"

#endif // Rice__protect__hpp_

