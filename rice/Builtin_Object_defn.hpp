#ifndef Rice__Builtin_Object_defn__hpp_
#define Rice__Builtin_Object_defn__hpp_

#include "Object_defn.hpp"
#include "Allocation_Strategies.hpp"
#include "detail/ruby.hpp"

namespace Rice
{

//! A smartpointer-like wrapper for Ruby builtin objects.
/*! A builtin object is one of Ruby's internal types, e.g. RArray or
 *  RString.  Every builtin type structure has a corresponding integer
 *  type number (e.g T_ARRAY for RArray or T_STRING for RString).  This
 *  class is a wrapper for those types of objects, primarily useful as a
 *  base class for other wrapper classes like Array and Hash.
 */
template<typename T, int Builtin_Type>
class Builtin_Object
  : public Object
{
public:
  //! Wrap an already allocated Ruby object.
  /*! Checks to see if the object is an object of type Builtin_Type; a
   *  C++ exception is thrown if this is not the case.
   *  \param value the object to be wrapped.
   */
  Builtin_Object(Object value);

  //! Make a copy of a Builtin_Object
  /*! \param other the Builtin_Object to be copied.
   */
  Builtin_Object(Builtin_Object const & other);

  T & operator*() const { return *obj_; } //!< Return a reference to obj_
  T * operator->() const { return obj_; } //!< Return a pointer to obj_
  T * get() const { return obj_; }        //!< Return a pointer to obj_

  //! Swap with another builtin object of the same type
  /*! \param ref the object with which to swap.
   */
  void swap(Builtin_Object<T, Builtin_Type> & ref);

private:
  T * obj_;
};

} // namespace Rice

#endif // Rice__Builtin_Object_defn__hpp_

