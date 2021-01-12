#ifndef Rice__to_from_ruby_defn__hpp_
#define Rice__to_from_ruby_defn__hpp_

#include "Object_defn.hpp"
#include "detail/from_ruby.hpp"
#include "detail/to_ruby.hpp"

#include <typeinfo>
#include <string>
#include <stdexcept>


//! Convert a C++ object to Ruby.
/*! If x is a pointer, wraps the pointee as a Ruby object.  If x is an
 *  Object, returns x.
 *
 *  If no conversion exists a compile-time error is generated.
 *
 *  \param x the object to convert.
 *  \return a Ruby representation of the C++ object.
 *
 *  Example:
 *  \code
 *    rb_p(to_ruby(42));
 *
 *    Foo * p_foo = new Foo();
 *    rb_p(to_ruby(p_foo));
 *  \endcode
 */
template<typename T>
inline
VALUE to_ruby(T const & x)
{
  return Rice::detail::to_ruby_<T>::convert(x);
}

#endif // Rice__to_from_ruby_defn__hpp_

