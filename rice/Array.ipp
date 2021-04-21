#ifndef Rice__Array__ipp_
#define Rice__Array__ipp_

namespace Rice
{
  inline Array::Array() : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
  }

  inline Array::Array(Object v) : Builtin_Object<T_ARRAY>(v)
  {
  }

  inline Array::Array(VALUE v) : Builtin_Object<T_ARRAY>(v)
  {
  }

  template<typename Iter_T>
  inline Array::Array(Iter_T it, Iter_T end) : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
    for (; it != end; ++it)
    {
      push(*it);
    }
  }

  template<typename T, long n>
  inline Array::Array(T const (&a)[n]) : Builtin_Object<T_ARRAY>(detail::protect(rb_ary_new))
  {
    for (long j = 0; j < n; ++j)
    {
      push(a[j]);
    }
  }

  inline long Array::size() const
  {
    return RARRAY_LEN(this->value());
  }

  inline Object Array::operator[](long index) const
  {
    return detail::protect(rb_ary_entry, value(), position_of(index));
  }

  inline Array::Proxy Array::operator[](long index)
  {
    return Proxy(*this, position_of(index));
  }

  template<typename T>
  inline Object Array::push(T const& obj)
  {
    return detail::protect(rb_ary_push, value(), detail::To_Ruby<T>::convert(obj, true));
  }

  inline Object Array::pop()
  {
    return detail::protect(rb_ary_pop, value());
  }

  template<typename T>
  inline Object Array::unshift(T const& obj)
  {
    return detail::protect(rb_ary_unshift, value(), detail::To_Ruby<T>::convert(obj, true));
  }

  inline Object Array::shift()
  {
    return detail::protect(rb_ary_shift, value());
  }

  inline long Array::position_of(long index) const
  {
    if (index < 0)
    {
      return size() + index;
    }
    else
    {
      return static_cast<long>(index);
    }
  }

  inline Array::Proxy::Proxy(Array array, long index)
    : array_(array)
    , index_(index)
  {
  }

  inline Array::Proxy::operator Object() const
  {
    return detail::protect(rb_ary_entry, array_.value(), index_);
  }

  inline VALUE Array::Proxy::value() const
  {
    return detail::protect(rb_ary_entry, array_.value(), index_);
  }

  template<typename T>
  Object Array::Proxy::operator=(T const& value)
  {
    Object o = detail::To_Ruby<T>::convert(value, true);
    detail::protect(rb_ary_store, array_.value(), index_, o.value());
    return o;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Array::Iterator<Array_Ptr_T, Value_T>::Iterator(Array_Ptr_T array, long index) :
    array_(array), index_(index)
  {
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline
    Array::Iterator<Array_Ptr_T, Value_T>::Iterator(Iterator<Array_Ptr_T_, Value_T_> const& rhs) :
      array_(rhs.array()) , index_(rhs.index()), tmp_()
  {
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline Array::Iterator<Array_Ptr_T, Value_T>& Array::Iterator<Array_Ptr_T, Value_T>::operator=(Iterator<Array_Ptr_T_, Value_T_> const& rhs)
  {
    array_ = rhs.array_;
    index_ = rhs.index_;
    return *this;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Array::Iterator<Array_Ptr_T, Value_T>& Array::Iterator<Array_Ptr_T, Value_T>::operator++()
  {
    ++index_;
    return *this;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Array::Iterator<Array_Ptr_T, Value_T> Array::Iterator<Array_Ptr_T, Value_T>::operator++(int)
  {
    Array copy(*this);
    ++(*this);
    return *this;
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Value_T Array::Iterator<Array_Ptr_T, Value_T>::operator*()
  {
    return (*array_)[index_];
  }

  template<typename Array_Ptr_T, typename Value_T>
  inline Object* Array::Iterator<Array_Ptr_T, Value_T>::operator->()
  {
    tmp_ = (*array_)[index_];
    return &tmp_;
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline bool Array::Iterator<Array_Ptr_T, Value_T>::operator==(Iterator<Array_Ptr_T_, Value_T_> const& rhs) const
  {
    return array_->value() == rhs.array_->value() && index_ == rhs.index_;
  }

  template<typename Array_Ptr_T, typename Value_T>
  template<typename Array_Ptr_T_, typename Value_T_>
  inline bool Array::Iterator<Array_Ptr_T, Value_T>::operator!=(Iterator<Array_Ptr_T_, Value_T_> const& rhs) const
  {
    return !(*this == rhs);
  }

  template<typename Array_Ptr_T, typename Value_T>
  Array_Ptr_T Array::Iterator<Array_Ptr_T, Value_T>::array() const
  {
    return array_;
  }

  template<typename Array_Ptr_T, typename Value_T>
  long Array::Iterator<Array_Ptr_T, Value_T>::index() const
  {
    return index_;
  }

  inline Array::iterator Array::begin()
  {
    return iterator(this, 0);
  }

  inline Array::const_iterator Array::begin() const
  {
    return const_iterator(this, 0);
  }

  inline Array::iterator Array::end()
  {
    return iterator(this, size());
  }

  inline Array::const_iterator Array::end() const
  {
    return const_iterator(this, size());
  }
}

namespace Rice::detail
{
  template<>
  class From_Ruby<Array>
  {
  public:
    Array convert(VALUE value)
    {
      return Array(value);
    }
  };

  template<>
  struct To_Ruby<Array>
  {
    static VALUE convert(Array const& x, bool takeOwnership = false)
    {
      return x.value();
    }
  };
}
#endif // Rice__Array__ipp_