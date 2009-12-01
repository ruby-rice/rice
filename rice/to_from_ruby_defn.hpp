#ifndef Rice__to_from_ruby_defn__hpp_
#define Rice__to_from_ruby_defn__hpp_

#include "Object_defn.hpp"
#include "detail/from_ruby.hpp"
#include "detail/to_ruby.hpp"

#include <typeinfo>
#include <string>
#include <stdexcept>

// ---------------------------------------------------------------------

//! Convert a Ruby object to C++.
/*! If the Ruby object can be converted to an immediate value, returns a
 *  copy of the Ruby object.  If the Ruby object is holding a C++
 *  object and the type specified is a pointer to that type, returns a
 *  pointer to that object.
 *
 *  Conversions from ruby to a pointer type are automatically generated
 *  when a type is bound using Data_Type.  If no conversion exists an
 *  exception is thrown.
 *
 *  \param T the C++ type to which to convert.
 *  \param x the Ruby object to convert.
 *  \return a C++ representation of the Ruby object.
 *
 *  Example:
 *  \code
 *    Object x = INT2NUM(42);
 *    std::cout << from_ruby<int>(x);
 *
 *    Data_Object<Foo> foo(new Foo);
 *    Object obj(foo);
 *    std::cout << *from_ruby<Foo *>(obj) << std::endl;
 *  \endcode
 */
template<typename T>
inline
typename Rice::detail::from_ruby_<T>::Retval_T from_ruby(Rice::Object x)
{
  return Rice::detail::from_ruby_<T>::convert(x);
}

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
 *    Data_Object<Foo> foo(new Foo);
 *    Foo * p_foo = foo.get();
 *    rb_p(to_ruby(p_foo));
 *  \endcode
 */
template<typename T>
inline
Rice::Object to_ruby(T const & x)
{
  return Rice::detail::to_ruby_<T>::convert(x);
}

#endif // Rice__to_from_ruby_defn__hpp_

