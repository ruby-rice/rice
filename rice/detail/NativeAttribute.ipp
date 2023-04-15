#include <array>
#include <algorithm>

#include "rice_traits.hpp"
#include "NativeRegistry.hpp"
#include "to_ruby_defn.hpp"
#include "cpp_protect.hpp"

namespace Rice::detail
{
  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::get(VALUE self)
  {
    return cpp_protect([&]
    {
      using Native_Attr_T = NativeAttribute<Return_T, Attr_T, Self_T>;
      Native_Attr_T* attr = detail::Registries::instance.natives.lookup<Native_Attr_T*>();
      return attr->read(self);
    });
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::set(VALUE self, VALUE value)
  {
    return cpp_protect([&]
    {
      using Native_Attr_T = NativeAttribute<Return_T, Attr_T, Self_T>;
      Native_Attr_T* attr = detail::Registries::instance.natives.lookup<Native_Attr_T*>();
      return attr->write(self, value);
    });
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  NativeAttribute<Return_T, Attr_T, Self_T>::NativeAttribute(VALUE klass, std::string name,
                                                             Attr_T attr, AttrAccess access)
    : klass_(klass), name_(name), attr_(attr), access_(access)
  {
    if (access == AttrAccess::ReadWrite || access == AttrAccess::Read)
    {
      // Define Ruby getter method
      ID method_id = Identifier(name).id();
      detail::protect(rb_define_method_id, klass, method_id, (RUBY_METHOD_FUNC)this->get, 0);
      detail::Registries::instance.natives.add(klass_, method_id, this);
    }

    if (access == AttrAccess::ReadWrite || access == AttrAccess::Write)
    {
      if (std::is_const_v<std::remove_pointer_t<Attr_T>>)
      {
        throw std::runtime_error(name + " is readonly");
      }

      // Define Ruby setter method
      ID method_id = Identifier(name + "=").id();
      detail::protect(rb_define_method_id, klass, method_id, (RUBY_METHOD_FUNC)this->set, 1);
      detail::Registries::instance.natives.add(klass_, method_id, this);
    }



  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::read(VALUE self)
  {
    using Unqualified_T = remove_cv_recursive_t<Return_T>;
    if constexpr (std::is_member_object_pointer_v<Attr_T>)
    {
      Self_T* nativeSelf = From_Ruby<Self_T*>().convert(self);
      return To_Ruby<Unqualified_T>().convert(nativeSelf->*attr_);
    }
    else
    {
      return To_Ruby<Unqualified_T>().convert(*attr_);
    }
  }

  template<typename Return_T, typename Attr_T, typename Self_T>
  inline VALUE NativeAttribute<Return_T, Attr_T, Self_T>::write(VALUE self, VALUE value)
  {
    if constexpr (std::is_fundamental_v<intrinsic_type<Attr_T>> && std::is_pointer_v<Attr_T>)
    {
      static_assert(true, "An fundamental value, such as an integer, cannot be assigned to an attribute that is a pointer");
    }
    else if constexpr (std::is_same_v<intrinsic_type<Attr_T>, std::string> && std::is_pointer_v<Attr_T>)
    {
      static_assert(true, "An string cannot be assigned to an attribute that is a pointer");
    }
    
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
  auto* Make_Native_Attribute(VALUE klass, std::string name, T* attr, AttrAccess access)
  {
    return new NativeAttribute<T, T*>(klass, name, attr, access);
  }

  template<typename Class_T, typename T>
  auto* Make_Native_Attribute(VALUE klass, std::string name, T Class_T::* attr, AttrAccess access)
  {
    using Attr_T = T Class_T::*;
    return new NativeAttribute<T, Attr_T, Class_T>(klass, name, attr, access);
  }
} // Rice
