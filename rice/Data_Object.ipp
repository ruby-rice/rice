#ifndef Rice__Data_Object__ipp_
#define Rice__Data_Object__ipp_

#include "protect.hpp"
#include "Data_Type_defn.hpp"

#include <algorithm>

template<typename T>
const typename Rice::Default_Mark_Function<T>::Ruby_Data_Func
Rice::Default_Mark_Function<T>::mark = ruby_mark<T>;

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(
    T* data,
    VALUE klass,
    Ruby_Data_Func mark_func,
    Ruby_Free_Func free_func)
{
  VALUE value = detail::wrap(klass,
                            (RUBY_DATA_FUNC)mark_func, 
                            (RUBY_DATA_FUNC)free_func, 
                            data);
  this->set_value(value);
}

template<typename T>
inline Rice::Data_Object<T>::
Data_Object(
    Object value)
  : Object(value)
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
{  
  check_cpp_type(klass);
  check_ruby_type(value, klass, true);
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

template<typename T>
inline T& Rice::Data_Object<T>::
operator*() const
{
  return *this->get();
}

template<typename T>
inline T* Rice::Data_Object<T>::
operator->() const
{
  return this->get();
}

template<typename T>
inline T* Rice::Data_Object<T>::
get() const
{
  if (this->value() == Qnil)
  {
    return nullptr;
  }
  else
  {
    return detail::unwrap<T>(this->value());
  }
}

template<typename T, typename>
struct Rice::detail::To_Ruby
{
  static VALUE convert(T&& data, bool takeOwnership)
  {
    using Base_T = base_type<T>;
    Data_Type<Base_T>::check_is_bound();
    return detail::wrap(Data_Type<Base_T>::klass(),
      (RUBY_DATA_FUNC)Default_Mark_Function<Base_T>::mark,
      (RUBY_DATA_FUNC)Default_Free_Function<Base_T>::free, std::forward<T>(data), takeOwnership);

  }
};

template <typename T>
struct Rice::detail::To_Ruby<T*, std::enable_if_t<!Rice::detail::is_primitive_v<T> &&
  !Rice::detail::is_kind_of_object<T>>>
{
  static VALUE convert(T* data, bool takeOwnership)
  {
    using Base_T = base_type<T>;
    return detail::wrap(Data_Type<Base_T>::klass(),
      (RUBY_DATA_FUNC)Default_Mark_Function<Base_T>::mark, 
      (RUBY_DATA_FUNC)Default_Free_Function<Base_T>::free, data, takeOwnership);
  }
};

template<typename T>
struct Rice::detail::From_Ruby
{
  static T& convert(VALUE value)
  {
    using Base_T = base_type<T>;
    Data_Type<Base_T>::check_descendant(value);
    return *detail::unwrap<T>(value);
  }
};

template<typename T>
struct Rice::detail::From_Ruby<T&>
{
  static T& convert(VALUE value)
  {
    using Base_T = base_type<T>;
    Data_Type<Base_T>::check_descendant(value);
    return *detail::unwrap<T>(value);
  }
};

template<typename T>
struct Rice::detail::From_Ruby<T*>
{
  static T* convert(VALUE value)
  {
    using Base_T = base_type<T>;
    Data_Type<Base_T>::check_descendant(value);
    return detail::unwrap<T>(value);
  }
};


#endif // Rice__Data_Object__ipp_

