#ifndef Rice__Array__ipp_
#define Rice__Array__ipp_

#include "protect.hpp"

inline Rice::Array::
Array()
  : Builtin_Object<RArray, T_ARRAY>(protect(rb_ary_new))
{
}

inline Rice::Array::
Array(Object v)
  : Builtin_Object<RArray, T_ARRAY>(v)
{
}

inline Rice::Array::
Array(VALUE v)
  : Builtin_Object<RArray, T_ARRAY>(v)
{
}

template<typename Iter_T>
inline Rice::Array::
Array(Iter_T it, Iter_T end)
  : Builtin_Object<RArray, T_ARRAY>(protect(rb_ary_new))
{
  for(; it != end; ++it)
  {
    push(*it);
  }
}

template<typename T, size_t n>
inline Rice::Array::
Array(T const (& a)[n])
  : Builtin_Object<RArray, T_ARRAY>(protect(rb_ary_new))
{
  for(size_t j = 0; j < n; ++j)
  {
    push(a[j]);
  }
}

inline size_t Rice::Array::
size() const
{
  return RARRAY_LEN(this->value());
}

inline Rice::Object Rice::Array::
operator[](ptrdiff_t index) const
{
  VALUE * ptr = RARRAY_PTR(this->value());
  return ptr[position_of(index)];
}

inline Rice::Array::Proxy Rice::Array::
operator[](ptrdiff_t index)
{
  return Proxy(*this, position_of(index));
}

template<typename T>
inline Rice::Object Rice::Array::
push(T const & obj)
{
  return protect(rb_ary_push, value(), to_ruby(obj));
}

inline Rice::Object Rice::Array::
pop()
{
  return protect(rb_ary_pop, value());
}

template<typename T>
inline Rice::Object Rice::Array::
unshift(T const & obj)
{
  return protect(rb_ary_unshift, value(), to_ruby(obj));
}

inline Rice::Object Rice::Array::
shift()
{
  return protect(rb_ary_shift, value());
}

inline VALUE * Rice::Array::
to_c_array()
{
  return RARRAY_PTR(this->value());
}

inline size_t Rice::Array::
position_of(ptrdiff_t index) const
{
  if(index < 0)
  {
    return size() + index;
  }
  else
  {
    return static_cast<size_t>(index);
  }
}

inline Rice::Array::Proxy::
Proxy(Array array, size_t index)
  : array_(array)
  , index_(index)
{
}

inline Rice::Array::Proxy::
operator Rice::Object() const
{
  return RARRAY_PTR(array_.value())[index_];
}

inline VALUE Rice::Array::Proxy::
value() const
{
  return RARRAY_PTR(array_.value())[index_];
}

template<typename T>
Rice::Object Rice::Array::Proxy::
operator=(T const & value)
{
  Object o = to_ruby(value);
  RARRAY_PTR(array_.value())[index_] = o.value();
  return o;
}

template<typename Array_Ref_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ref_T, Value_T>::
Iterator(Array_Ref_T array, size_t index)
  : array_(array)
  , index_(index)
{
}

template<typename Array_Ref_T, typename Value_T>
template<typename Array_Ref_T_, typename Value_T_>
inline
Rice::Array::Iterator<Array_Ref_T, Value_T>::
Iterator(Iterator<Array_Ref_T_, Value_T_> const & rhs)
  : array_(rhs.array())
  , index_(rhs.index())
  , tmp_()
{
}

template<typename Array_Ref_T, typename Value_T>
template<typename Array_Ref_T_, typename Value_T_>
inline Rice::Array::Iterator<Array_Ref_T, Value_T> &
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator=(Iterator<Array_Ref_T_, Value_T_> const & rhs)
{
  array_ = rhs.array_;
  index_ = rhs.index_;
  return *this;
}

template<typename Array_Ref_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ref_T, Value_T> &
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator++()
{
  ++index_;
  return *this;
}

template<typename Array_Ref_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ref_T, Value_T>
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator++(int)
{
  Array copy(*this);
  ++(*this);
  return *this;
}

template<typename Array_Ref_T, typename Value_T>
inline Value_T
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator*()
{
  return array_[index_];
}

template<typename Array_Ref_T, typename Value_T>
inline Rice::Object *
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator->()
{
  tmp_ = array_[index_];
  return &tmp_;
}

template<typename Array_Ref_T, typename Value_T>
template<typename Array_Ref_T_, typename Value_T_>
inline bool
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator==(Iterator<Array_Ref_T_, Value_T_> const & rhs) const
{
  return array_.value() == rhs.array_.value() && index_ == rhs.index_;
}

template<typename Array_Ref_T, typename Value_T>
template<typename Array_Ref_T_, typename Value_T_>
inline bool
Rice::Array::Iterator<Array_Ref_T, Value_T>::
operator!=(Iterator<Array_Ref_T_, Value_T_> const & rhs) const
{
  return !(*this == rhs);
}

template<typename Array_Ref_T, typename Value_T>
Array_Ref_T
Rice::Array::Iterator<Array_Ref_T, Value_T>::
array() const
{
  return array_;
}

template<typename Array_Ref_T, typename Value_T>
size_t
Rice::Array::Iterator<Array_Ref_T, Value_T>::
index() const
{
  return index_;
}

inline Rice::Array::iterator Rice::Array::
begin()
{
  return iterator(*this, 0);
}

inline Rice::Array::const_iterator Rice::Array::
begin() const
{
  return const_iterator(*this, 0);
}

inline Rice::Array::iterator Rice::Array::
end()
{
  return iterator(*this, size());
}

inline Rice::Array::const_iterator Rice::Array::
end() const
{
  return const_iterator(*this, size());
}

#endif // Rice__Array__ipp_

