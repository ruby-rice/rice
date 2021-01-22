#ifndef Rice__Array__hpp_
#define Rice__Array__hpp_

#include "Builtin_Object.hpp"
#include "detail/ruby.hpp"
#include <iterator>

namespace Rice
{

//! A wrapper for the ruby Array class.
/*! This class provides a C++-style interface to ruby's Array class and
 *  its associated rb_ary_* functions.
 *  Example:
 *  \code
 *    Array a;
 *    a.push(String("some string"));
 *    a.push(42);
 *  \endcode
 */
class Array
  : public Builtin_Object<T_ARRAY>
{
public:
  //! Construct a new array
  Array();

  //! Wrap an existing array
  /*! \param v a ruby object, which must be of type T_ARRAY.
   */
  Array(Object v);

  //! Wrap an existing array
  /*! \param v a ruby object, which must be of type T_ARRAY.
   */
  Array(VALUE v);

  //! Construct an array from a sequence.
  /*! \param begin an iterator to the beginning of the sequence.
   *  \param end an iterator to the end of the sequence.
   */
  template<typename Iter_T>
  Array(Iter_T begin, Iter_T end);

  //! Construct an Array from a C array.
  /*! \param a a C array of type T and size n.
   */
  template<typename T, long n>
  Array(T const (& a)[n]);

public:
  //! Return the size of the array.
  long size() const;

  //! Return the element at the given index.
  /*! \param index The index of the desired element.  The index may be
   *  negative, to indicate an offset from the end of the array.  If the
   *  index is out of bounds, this function has undefined behavior.
   *  \return the element at the given index.
   */
  Object operator[](long index) const;

private:
  //! A helper class so array[index]=value can work.
  class Proxy;

public:
  //! Return a reference to the element at the given index.
  /*! \param index The index of the desired element.  The index may be
   *  negative, to indicate an offset from the end of the array.  If the
   *  index is out of bounds, this function has undefined behavior.
   *  \return the element at the given index.
   */
  Proxy operator[](long index);

  //! Push an element onto the end of the array
  /*! \param v an object to push onto the array.
   *  \return the object which was pushed onto the array.
   */
  template<typename T>
  Object push(T const & obj);

  //! Pop an element from the end of the array
  /*! \return the object which was popped from the array, or Qnil if
   *  the array was empty.
   */
  Object pop();

  //! Unshift an element onto the beginning of the array
  /*! \param v an object to unshift onto the array.
   *  \return the object which was unshifted onto the array.
   */
  template<typename T>
  Object unshift(T const & obj);

  //! Shift an element from the beginning of the array
  /*! \return the object which was shifted from the array.
   */
  Object shift();

private:
  template<typename Array_Ptr_T, typename Value_T>
  class Iterator;

  long position_of(long index) const;

public:
  //! An iterator.
  typedef Iterator<Array *, Proxy> iterator;

  //! A const iterator.
  typedef Iterator<Array const *, Object> const_iterator;

  //! Return an iterator to the beginning of the array.
  iterator begin();

  //! Return a const iterator to the beginning of the array.
  const_iterator begin() const;

  //! Return an iterator to the end of the array.
  iterator end();

  //! Return a const iterator to the end of the array.
  const_iterator end() const;
};

//! A helper class so array[index]=value can work.
class Array::Proxy
{
public:
  //! Construct a new Proxy
  Proxy(Array array, long index);

  //! Implicit conversion to Object.
  operator Object() const;

  //! Explicit conversion to VALUE.
  VALUE value() const;

  //! Assignment operator.
  template<typename T>
  Object operator=(T const & value);

private:
  Array array_;
  long index_;
};

//! A helper class for implementing iterators for a Array.
// TODO: This really should be a random-access iterator.
template<typename Array_Ptr_T, typename Value_T>
class Array::Iterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = Value_T;
  using difference_type = long;
  using pointer = Object*;
  using reference = Value_T&;  
  
  Iterator(Array_Ptr_T array, long index);

  template<typename Array_Ptr_T_, typename Value_T_>
  Iterator(Iterator<Array_Ptr_T_, Value_T_> const & rhs);

  template<typename Array_Ptr_T_, typename Value_T_>
  Iterator & operator=(Iterator<Array_Ptr_T_, Value_T_> const & rhs);

  Iterator & operator++();
  Iterator operator++(int);
  Value_T operator*();
  Object * operator->();

  template<typename Array_Ptr_T_, typename Value_T_>
  bool operator==(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const;

  template<typename Array_Ptr_T_, typename Value_T_>
  bool operator!=(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const;

  Array_Ptr_T array() const;
  long index() const;

private:
  Array_Ptr_T array_;
  long index_;

  Object tmp_;
};

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Array>
{
  static Rice::Array convert(VALUE value)
  {
    return Rice::Array(value);
  }
};

#include "Array.ipp"

#endif // Rice__Array__hpp_

