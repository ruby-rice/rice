#ifndef Rice__Data_Object_defn__hpp_
#define Rice__Data_Object_defn__hpp_

#include "Object_defn.hpp"
#include "Data_Type_fwd.hpp"
#include "Allocation_Strategies.hpp"
#include "ruby_mark.hpp"
#include "detail/to_ruby.hpp"
#include "detail/ruby.hpp"

/*! \file
 *  \brief Provides a helper class for wrapping and unwrapping C++
 *  objects as Ruby objects.
 */

namespace Rice
{

template<typename T>
struct Default_Mark_Function
{
  typedef void (*Ruby_Data_Func)(T * obj);
  static const Ruby_Data_Func mark;
};

//! A smartpointer-like wrapper for Ruby data objects.
/*! A data object is a ruby object of type T_DATA, which is usually
 *  created by using the Data_Wrap_Struct or Data_Make_Struct macro.
 *  This class wraps creation of the data structure, providing a
 *  type-safe object-oriented interface to the underlying C interface.
 *  This class works in conjunction with the Data_Type class to ensure
 *  type safety.
 *
 *  Example:
 *  \code
 *    class Foo { };
 *    ...
 *    Data_Type<Foo> rb_cFoo = define_class("Foo");
 *    ...
 *    // Wrap:
 *    Data_Object<Foo> foo1(new Foo);
 *
 *    // Get value to return:
 *    VALUE v = foo1.value()
 *
 *    // Unwrap:
 *    Data_Object<Foo> foo2(v, rb_cFoo);
 *  \endcode
 */
template<typename T>
class Data_Object
  : public Object
{
public:
  //! A function that takes a T* and returns void.
  typedef void (*Ruby_Data_Func)(T * obj);

  //! Wrap a C++ object.
  /*! This constructor is analgous to calling Data_Wrap_Struct.  Be
   *  careful not to call this function more than once for the same
   *  pointer (in general, it should only be called for newly
   *  constructed objects that need to be managed by Ruby's garbage
   *  collector).
   *  \param obj the object to wrap.
   *  \param klass the Ruby class to use for the newly created Ruby
   *  object.
   *  \param mark_func a function that gets called by the garbage
   *  collector to mark the object's children.
   *  \param free_func a function that gets called by the garbage
   *  collector to free the object.
   */
  Data_Object(
      T * obj,
      VALUE klass = Data_Type<T>::klass(),
      Ruby_Data_Func mark_func = Default_Mark_Function<T>::mark,
      Ruby_Data_Func free_func = Default_Allocation_Strategy<T>::free);

  //! Unwrap a Ruby object.
  /*! This constructor is analgous to calling Data_Get_Struct.  Uses
   *  Data_Type<T>::klass as the class of the object.
   *  \param value the Ruby object to unwrap.
   */
  Data_Object(
      Object value);

  //! Unwrap a Ruby object.
  /*! This constructor is analgous to calling Data_Get_Struct.  Will
   *  throw an exception if the class of the object differs from the
   *  specified class.
   *  \param value the Ruby object to unwrap.
   *  \param klass the expected class of the object.
   */
  template<typename U>
  Data_Object(
      Object value,
      Data_Type<U> const & klass = Data_Type<T>::klass());

  //! Make a copy of a Data_Object
  /*! \param other the Data_Object to copy.
   */
  Data_Object(Data_Object const & other);

  T & operator*() const { return *obj_; } //!< Return a reference to obj_
  T * operator->() const { return obj_; } //!< Return a pointer to obj_
  T * get() const { return obj_; }        //!< Return a pointer to obj_

  //! Swap with another data object of the same type
  /*! \param ref the object with which to swap.
   */
  template<typename U>
  void swap(Data_Object<U> & ref);

private:
  static void check_cpp_type(Data_Type<T> const & klass);

private:
  T * obj_;
};

namespace detail
{
  template<typename T>
  struct to_ruby_<Data_Object<T> >
  {
    static Rice::Object convert(Data_Object<T> const & x);
  };
}

} // namespace Rice

#endif // Rice__Data_Object_defn__hpp_

