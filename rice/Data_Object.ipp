#ifndef Rice__Data_Object__ipp_
#define Rice__Data_Object__ipp_

#include "Data_Type_defn.hpp"

#include <algorithm>

namespace Rice
{
  template <typename T>
  Exception create_type_exception(VALUE value)
  {
    return Exception(rb_eTypeError, "Wrong argument type. Expected: %s. Received: %s.",
      detail::protect(rb_class2name, Data_Type<T>::klass().value()),
      detail::protect(rb_obj_classname, value));
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(T& data, bool isOwner, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::rb_type(), data, isOwner);
    this->set_value(value);
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(T* data, bool isOwner, Class klass)
  {
    VALUE value = detail::wrap(klass, Data_Type<T>::rb_type(), data, isOwner);
    this->set_value(value);
  }

  template<typename T>
  inline Data_Object<T>::Data_Object(Object value) : Object(value)
  {
    Data_Type<T> klass;
    check_ruby_type(value);
  }

  template<typename T>
  template<typename U>
  inline Data_Object<T>::Data_Object(Object value) : Object(value)
  {
    check_ruby_type(value);
  }

  template<typename T>
  inline void Data_Object<T>::check_ruby_type(VALUE value)
  {
    if (rb_obj_is_kind_of(value, Data_Type<T>::klass()) == Qfalse)
    {
      throw create_type_exception<T>(value);
    }
  }

  template<typename T>
  inline T& Data_Object<T>::operator*() const
  {
    return *this->get();
  }

  template<typename T>
  inline T* Data_Object<T>::operator->() const
  {
    return this->get();
  }

  template<typename T>
  inline T* Data_Object<T>::get() const
  {
    if (this->value() == Qnil)
    {
      return nullptr;
    }
    else
    {
      return detail::unwrap<T>(this->value(), Data_Type<T>::rb_type());
    }
  }

  template<typename T>
  inline T* Data_Object<T>::from_ruby(VALUE value)
  {
    if (Data_Type<T>::is_descendant(value))
    {
      return detail::unwrap<T>(value, Data_Type<T>::rb_type());
    }
    else
    {
      throw create_type_exception<T>(value);
    }
  }
}

template<typename T, typename>
struct Rice::detail::To_Ruby
{
  static VALUE convert(T& data, bool isOwner)
  {
    // Note that T could be a pointer or reference to a base class while data is in fact a
    // child class. Lookup the correct type so we return an instance of the correct Ruby class
    std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::TypeRegistry::figureType<T>(data);
    return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
  }
};

template <typename T>
struct Rice::detail::To_Ruby<const T&, std::enable_if_t<!Rice::detail::is_builtin_v<T>>>
{
  static VALUE convert(const T& data, bool isOwner)
  {
    // Note that T could be a pointer or reference to a base class while data is in fact a
    // child class. Lookup the correct type so we return an instance of the correct Ruby class
    std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::TypeRegistry::figureType<T>(data);
    return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
  }
};

template <typename T>
struct Rice::detail::To_Ruby<T*, std::enable_if_t<!Rice::detail::is_builtin_v<T>>>
{
  static VALUE convert(T* data, bool isOwner)
  {
    if (data)
    {
      // Note that T could be a pointer or reference to a base class while data is in fact a
      // child class. Lookup the correct type so we return an instance of the correct Ruby class
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::TypeRegistry::figureType(*data);
      return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
    }
    else
    {
      return Qnil;
    }
  }
};

template <typename T, typename>
struct Rice::detail::From_Ruby
{
  static T convert(VALUE value)
  {
    using Intrinsic_T = intrinsic_type<T>;
    return *Data_Object<Intrinsic_T>::from_ruby(value);
  }
};
 
template<typename T>
struct Rice::detail::From_Ruby<T&>
{
  static T& convert(VALUE value)
  {
    using Intrinsic_T = intrinsic_type<T>;
    return *Data_Object<Intrinsic_T>::from_ruby(value);
  }
};

namespace Rice::detail
{
  template<typename T>
  struct From_Ruby<T*>
  {
    static T* convert(VALUE value)
    {
      using Intrinsic_T = intrinsic_type<T>;

      if (value == Qnil)
      {
        return nullptr;
      }
      else
      {
        return Data_Object<Intrinsic_T>::from_ruby(value);
      }
    }
  };

  template<typename T>
  struct From_Ruby<Data_Object<T>>
  {
    static Data_Object<T> convert(VALUE value)
    {
      return Data_Object<T>(value);
    }
  };

  template<typename T>
  struct To_Ruby<Data_Object<T>>
  {
    static VALUE convert(Object const& x, bool takeOwnership = false)
    {
      return x.value();
    }
  };
}
#endif // Rice__Data_Object__ipp_