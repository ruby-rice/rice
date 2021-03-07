#ifndef Rice__Array__ipp_
#define Rice__Array__ipp_

inline Rice::Array::
Array()
  : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
{
}

inline Rice::Array::
Array(Object v)
  : Builtin_Object<T_ARRAY>(v)
{
}

inline Rice::Array::
Array(VALUE v)
  : Builtin_Object<T_ARRAY>(v)
{
}

template<typename Iter_T>
inline Rice::Array::
Array(Iter_T it, Iter_T end)
  : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
{
  for(; it != end; ++it)
  {
    push(*it);
  }
}

template<typename T, long n>
inline Rice::Array::
Array(T const (& a)[n])
  : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
{
  for(long j = 0; j < n; ++j)
  {
    push(a[j]);
  }
}

inline long Rice::Array::
size() const
{
  return RARRAY_LEN(this->value());
}

inline Rice::Object Rice::Array::
operator[](long index) const
{
  return detail::protect(rb_ary_entry, value(), position_of(index));
}

inline Rice::Array::Proxy Rice::Array::
operator[](long index)
{
  return Proxy(*this, position_of(index));
}

template<typename T>
inline Rice::Object Rice::Array::
push(T const & obj)
{
  return detail::protect(rb_ary_push, value(), detail::To_Ruby<T>::convert(obj));
}

inline Rice::Object Rice::Array::
pop()
{
  return detail::protect(rb_ary_pop, value());
}

template<typename T>
inline Rice::Object Rice::Array::
unshift(T const & obj)
{
  return detail::protect(rb_ary_unshift, value(), detail::To_Ruby<T>::convert(obj));
}

inline Rice::Object Rice::Array::
shift()
{
  return detail::protect(rb_ary_shift, value());
}

inline long Rice::Array::
position_of(long index) const
{
  if(index < 0)
  {
    return size() + index;
  }
  else
  {
    return static_cast<long>(index);
  }
}

inline Rice::Array::Proxy::
Proxy(Array array, long index)
  : array_(array)
  , index_(index)
{
}

inline Rice::Array::Proxy::
operator Rice::Object() const
{
  return detail::protect(rb_ary_entry, array_.value(), index_);
}

inline VALUE Rice::Array::Proxy::
value() const
{
  return detail::protect(rb_ary_entry, array_.value(), index_);
}

template<typename T>
Rice::Object Rice::Array::Proxy::
operator=(T const & value)
{
  Object o = detail::To_Ruby<T>::convert(value);
  rb_ary_store(array_.value(), index_, o.value());
  return o;
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T>::
Iterator(Array_Ptr_T array, long index)
  : array_(array)
  , index_(index)
{
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
Iterator(Iterator<Array_Ptr_T_, Value_T_> const & rhs)
  : array_(rhs.array())
  , index_(rhs.index())
  , tmp_()
{
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T> &
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator=(Iterator<Array_Ptr_T_, Value_T_> const & rhs)
{
  array_ = rhs.array_;
  index_ = rhs.index_;
  return *this;
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T> &
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator++()
{
  ++index_;
  return *this;
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Array::Iterator<Array_Ptr_T, Value_T>
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator++(int)
{
  Array copy(*this);
  ++(*this);
  return *this;
}

template<typename Array_Ptr_T, typename Value_T>
inline Value_T
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator*()
{
  return (*array_)[index_];
}

template<typename Array_Ptr_T, typename Value_T>
inline Rice::Object *
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator->()
{
  tmp_ = (*array_)[index_];
  return &tmp_;
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline bool
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator==(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const
{
  return array_->value() == rhs.array_->value() && index_ == rhs.index_;
}

template<typename Array_Ptr_T, typename Value_T>
template<typename Array_Ptr_T_, typename Value_T_>
inline bool
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
operator!=(Iterator<Array_Ptr_T_, Value_T_> const & rhs) const
{
  return !(*this == rhs);
}

template<typename Array_Ptr_T, typename Value_T>
Array_Ptr_T
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
array() const
{
  return array_;
}

template<typename Array_Ptr_T, typename Value_T>
long
Rice::Array::Iterator<Array_Ptr_T, Value_T>::
index() const
{
  return index_;
}

inline Rice::Array::iterator Rice::Array::
begin()
{
  return iterator(this, 0);
}

inline Rice::Array::const_iterator Rice::Array::
begin() const
{
  return const_iterator(this, 0);
}

inline Rice::Array::iterator Rice::Array::
end()
{
  return iterator(this, size());
}

inline Rice::Array::const_iterator Rice::Array::
end() const
{
  return const_iterator(this, size());
}

#endif // Rice__Array__ipp_

