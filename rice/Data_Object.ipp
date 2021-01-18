#ifndef Rice__Data_Object__ipp_
#define Rice__Data_Object__ipp_

#include "protect.hpp"
#include "Data_Type_defn.hpp"

#include <algorithm>

template<typename T>
const typename Rice::Default_Mark_Function<T>::Ruby_Data_Func
Rice::Default_Mark_Function<T>::mark = ruby_mark<T>;

namespace Rice
{

namespace detail
{

inline VALUE data_wrap_struct(
    VALUE klass,
    RUBY_DATA_FUNC mark,
    RUBY_DATA_FUNC free,
    void * obj)
{
  return Data_Wrap_Struct(klass, mark, free, obj);
}

template<typename T>
inline VALUE wrap(
    VALUE klass,
    typename Data_Object<T>::Ruby_Data_Func mark,
    typename Data_Object<T>::Ruby_Data_Func free,
    T * obj)
{
  // We cast to obj void* here before passing to Data_Wrap_Struct,
  // becuase otherwise compilation will fail if obj is const.  It's safe
  // to do this, because unwrap() will always add the const back when
  // the object is unwrapped.
  return Rice::protect(data_wrap_struct,
      klass,
      reinterpret_cast<RUBY_DATA_FUNC>(mark),
      reinterpret_cast<RUBY_DATA_FUNC>(free),
      (void *)obj);
}

template<typename T>
inline VALUE data_get_struct(VALUE value, T * * obj)
{
  Data_Get_Struct(value, T, *obj);
  return Qnil;
}

template<typename T>
inline T * unwrap(VALUE value)
{
  T * obj;
  Rice::protect(data_get_struct<T>, value, &obj);
  return obj;
}

} // namespace detail

} // namespace Rice

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(
    T * obj,
    VALUE klass,
    Ruby_Data_Func mark_func,
    Ruby_Data_Func free_func)
  : obj_(obj)
{
  if (klass == Qnil)
  {
    klass = Data_Type<T>::klass();
  }

  VALUE value = detail::wrap(klass, mark_func, free_func, obj);
  this->set_value(value);
}

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(
    Object value)
  : Object(value)
  , obj_(detail::unwrap<T>(value))
{  
  Data_Type<T> klass;
  check_cpp_type(klass);
  check_ruby_type(value, klass, true);
}

template<typename T>
template<typename U>
inline Rice::Data_Object<T>::
Data_Object(
    Object value,
    Data_Type<U> const & klass)
  : Object(value)
  , obj_(detail::unwrap<T>(value))
{  
  check_cpp_type(klass);
  check_ruby_type(value, klass, true);
}

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(Data_Object const & other)
  : Object(other.value())
  , obj_(other.obj_)
{
}

template<typename T>
template<typename U>
inline void Rice::Data_Object<T>::
swap(Data_Object<U> & ref)
{
  std::swap(obj_, ref.obj_);
  Object::swap(ref);
}

template<typename T>
inline void Rice::Data_Object<T>::
check_cpp_type(Data_Type<T> const & /* klass */)
{
}

template<typename T>
inline void Rice::Data_Object<T>::
check_ruby_type(VALUE value, VALUE klass, bool include_super)
{
  if (!rb_obj_is_kind_of(value, klass) || (!include_super && !rb_obj_is_instance_of(value, klass)))
  {
    // Not sure why this stuff can't be chained
    VALUE gotV = protect(rb_class_name, rb_obj_class(value));
    char* got = StringValueCStr(gotV);
    VALUE exptV = protect(rb_class_name, klass);
    char* expected = StringValueCStr(exptV);

    throw Exception(
      rb_eTypeError,
      "wrong argument type %s (expected %s)",
      got, expected
    );
  }
}

#endif // Rice__Data_Object__ipp_

