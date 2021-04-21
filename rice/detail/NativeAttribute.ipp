#include <array>
#include <algorithm>

#include "rice_traits.hpp"
#include "method_data.hpp"
#include "to_ruby_defn.hpp"
#include "../ruby_try_catch.hpp"

namespace Rice::detail
{
  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::get(VALUE self)
  {
    RUBY_TRY
    {
      using Native_Attr_T = NativeAttribute<Return_T, Attr_T, Self_T>;
      Native_Attr_T* attr = detail::MethodData::data<Native_Attr_T*>();
      return attr->read(self);
    }
    RUBY_CATCH
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::set(VALUE self, VALUE value)
  {
    RUBY_TRY
    {
      using Native_Attr_T = NativeAttribute<Return_T, Attr_T, Self_T>;
      Native_Attr_T* attr = detail::MethodData::data<Native_Attr_T*>();
      return attr->write(self, value);
    }
    RUBY_CATCH
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  NativeAttribute<Return_T, Attr_T, Self_T>::NativeAttribute(Attr_T attr, AttrAccess access)
    : attr_(attr), access_(access)
  {
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::read(VALUE self)
  {
    if constexpr (std::is_member_object_pointer_v<Attr_T>)
    {
      Self_T* nativeSelf = From_Ruby<Self_T*>().convert(self);
      return To_Ruby<Return_T>().convert(nativeSelf->*attr_, false);
    }
    else
    {
      return To_Ruby<Return_T>().convert(*attr_, false);
    }
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::write(VALUE self, VALUE value)
  {
    if constexpr (!std::is_const_v<std::remove_pointer_t<Attr_T>> && std::is_member_object_pointer_v<Attr_T>)
    {
      Self_T* nativeSelf = From_Ruby<Self_T*>().convert(self);
      nativeSelf->*attr_ = From_Ruby<Return_T>().convert(value);
    }
    else if constexpr (!std::is_const_v<std::remove_pointer_t<Attr_T>>)
    {
      *attr_ = From_Ruby<Return_T>().convert(value);
    }
    return value;
  }

  template<typename T>
  auto* Make_Native_Attribute(T* attr, AttrAccess access)
  {
    return new NativeAttribute<T, T*>(attr, access);
  }

  template<typename Class_T, typename T>
  auto* Make_Native_Attribute(T Class_T::* attr, AttrAccess access)
  {
    using Attr_T = T Class_T::*;
    return new NativeAttribute<T, Attr_T, Class_T>(attr, access);
  }
} // Rice
