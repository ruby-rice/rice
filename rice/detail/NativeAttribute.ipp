#include <array>
#include <algorithm>

#include "../traits/rice_traits.hpp"
#include "NativeRegistry.hpp"
#include "to_ruby_defn.hpp"
#include "cpp_protect.hpp"

namespace Rice::detail
{
  template<typename Attribute_T>
  inline VALUE NativeAttribute<Attribute_T>::get(VALUE self)
  {
    return cpp_protect([&]
    {
      using Native_Attr_T = NativeAttribute<Attribute_T>;
      Native_Attr_T* attr = detail::Registries::instance.natives.lookup<Native_Attr_T*>();
      return attr->read(self);
    });
  }

  template<typename Attribute_T>
  inline VALUE NativeAttribute<Attribute_T>::set(VALUE self, VALUE value)
  {
    return cpp_protect([&]
    {
      using Native_Attr_T = NativeAttribute<Attribute_T>;
      Native_Attr_T* attr = detail::Registries::instance.natives.lookup<Native_Attr_T*>();
      return attr->write(self, value);
    });
  }

  template<typename Attribute_T>
  NativeAttribute<Attribute_T>::NativeAttribute(VALUE klass, std::string name,
                                                             Attribute_T attr, AttrAccess access)
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
      if (std::is_const_v<std::remove_pointer_t<T>>)
      {
        throw std::runtime_error(name + " is readonly");
      }

      // Define Ruby setter method
      ID method_id = Identifier(name + "=").id();
      detail::protect(rb_define_method_id, klass, method_id, (RUBY_METHOD_FUNC)this->set, 1);
      detail::Registries::instance.natives.add(klass_, method_id, this);
    }
  }

  template<typename Attribute_T>
  inline VALUE NativeAttribute<Attribute_T>::read(VALUE self)
  {
    using T_Unqualified = remove_cv_recursive_t<T>;
    if constexpr (std::is_member_object_pointer_v<Attribute_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);
      return To_Ruby<T_Unqualified>().convert(nativeSelf->*attr_);
    }
    else
    {
      return To_Ruby<T_Unqualified>().convert(*attr_);
    }
  }

  template<typename Attribute_T>
  inline VALUE NativeAttribute<Attribute_T>::write(VALUE self, VALUE value)
  {
    if constexpr (std::is_fundamental_v<intrinsic_type<T>> && std::is_pointer_v<T>)
    {
      static_assert(true, "An fundamental value, such as an integer, cannot be assigned to an attribute that is a pointer");
    }
    else if constexpr (std::is_same_v<intrinsic_type<T>, std::string> && std::is_pointer_v<T>)
    {
      static_assert(true, "An string cannot be assigned to an attribute that is a pointer");
    }
    
    if constexpr (!std::is_null_pointer_v<Receiver_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);
      nativeSelf->*attr_ = From_Ruby<T_Unqualified>().convert(value);
    }
    else if constexpr (!std::is_const_v<std::remove_pointer_t<T>>)
    {
      *attr_ = From_Ruby<T_Unqualified>().convert(value);
    }

    return value;
  }
} // Rice
