#ifndef Rice__Array__hpp_
#define Rice__Array__hpp_

#include "Builtin_Object.hpp"
#include "to_from_ruby_defn.hpp"
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
  : public Builtin_Object<RArray, T_ARRAY>
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
  template<typename T, size_t n>
  Array(T const (& a)[n]);

public:
  //! Return the size of the array.
  size_t size() const;

  //! Return the element at the given index.
  /*! \param index The index of the desired element.  The index may be
   *  negative, to indicate an offset from the end of the array.  If the
   *  index is out of bounds, this function has undefined behavior.
   *  \return the element at the given index.
   */
  Object operator[](ptrdiff_t index) const;

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
  Proxy operator[](ptrdiff_t index);

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

  //! Return a pointer to the beginning of the underlying C array.
  //! Use with caution!
  /*! \return a pointer to the beginning of the array.
   */
  VALUE * to_c_array();

private:
  template<typename Array_Ref_T, typename Value_T>
  class Iterator;

  size_t position_of(ptrdiff_t index) const;

public:
  //! An iterator.
  typedef Iterator<Array &, Proxy> iterator;

  //! A const iterator.
  typedef Iterator<Array const &, Object> const_iterator;

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
  Proxy(Array array, size_t index);

  //! Implicit conversion to Object.
  operator Object() const;

  //! Explicit conversion to VALUE.
  VALUE value() const;

  //! Assignment operator.
  template<typename T>
  Object operator=(T const & value);

private:
  Array array_;
  size_t index_;
};

//! A helper class for implementing iterators for a Array.
// TODO: This really should be a random-access iterator.
template<typename Array_Ref_T, typename Value_T>
class Array::Iterator
  : public std::iterator<
      std::forward_iterator_tag,
      Value_T,            // Type
      ptrdiff_t,          // Distance type
      Object *,           // Pointer type
      Value_T &>          // Reference type
{
public:
  Iterator(Array_Ref_T array, size_t index);

  template<typename Array_Ref_T_, typename Value_T_>
  Iterator(Iterator<Array_Ref_T_, Value_T_> const & rhs);

  template<typename Array_Ref_T_, typename Value_T_>
  Iterator & operator=(Iterator<Array_Ref_T_, Value_T_> const & rhs);

  Iterator & operator++();
  Iterator operator++(int);
  Value_T operator*();
  Object * operator->();

  template<typename Array_Ref_T_, typename Value_T_>
  bool operator==(Iterator<Array_Ref_T_, Value_T_> const & rhs) const;

  template<typename Array_Ref_T_, typename Value_T_>
  bool operator!=(Iterator<Array_Ref_T_, Value_T_> const & rhs) const;

  // Causes ICE on g++ 3.3.3
  // template<typename Array_Ref_T_, typename Value_T_>
  // friend class Iterator;

  Array_Ref_T array() const;
  size_t index() const;

private:
  Array_Ref_T array_;
  size_t index_;

  Object tmp_;
};

} // namespace Rice

template<>
inline
Rice::Array from_ruby<Rice::Array>(Rice::Object x)
{
  return Rice::Array(x);
}

template<>
inline
Rice::Object to_ruby<Rice::Array>(Rice::Array const & x)
{
  return x;
}

#include "Array.ipp"

#endif // Rice__Array__hpp_

