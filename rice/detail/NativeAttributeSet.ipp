#include <array>
#include <algorithm>

#include "../traits/rice_traits.hpp"
#include "NativeRegistry.hpp"
#include "to_ruby_defn.hpp"
#include "cpp_protect.hpp"

namespace Rice::detail
{
  template<typename Attribute_T>
  void NativeAttributeSet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute)
  {
    // Create a NativeAttributeSet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute));
    std::unique_ptr<Native> native(nativeAttribute);

    if (std::is_const_v<std::remove_pointer_t<T>>)
    {
      throw std::runtime_error(name + " is readonly");
    }

    // Define the write method name
    std::string setter = name + "=";

    // Tell Ruby to invoke the static method write to get the attribute value
    detail::protect(rb_define_method, klass, setter.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry. Since attributes cannot be overridden, there is no need to set the
    // matches or calls function pointer. Instead Ruby can call the static call method defined on
    // this class (&NativeAttribute_T::set).
    Identifier identifier(setter);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  NativeAttributeSet<Attribute_T>::NativeAttributeSet(VALUE klass, std::string name, Attribute_T attribute)
    : klass_(klass), name_(name), attribute_(attribute)
  {
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeSet<Attribute_T>::matches(int argc, VALUE* argv, VALUE self)
  {
    if (argc == 1)
      return Resolved{ Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeSet<Attribute_T>::operator()(int argc, VALUE* argv, VALUE self)
  {
    if constexpr (std::is_fundamental_v<intrinsic_type<T>> && std::is_pointer_v<T>)
    {
      static_assert(true, "An fundamental value, such as an integer, cannot be assigned to an attribute that is a pointer");
    }
    else if constexpr (std::is_same_v<intrinsic_type<T>, std::string> && std::is_pointer_v<T>)
    {
      static_assert(true, "An string cannot be assigned to an attribute that is a pointer");
    }

    if (argc != 1)
    {
      throw std::runtime_error("Incorrect number of parameter set to attribute writer");
    }

    VALUE value = argv[0];
    
    if constexpr (!std::is_null_pointer_v<Receiver_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);
      nativeSelf->*attribute_ = From_Ruby<T_Unqualified>().convert(value);
    }
    else if constexpr (!std::is_const_v<std::remove_pointer_t<T>>)
    {
      *attribute_ = From_Ruby<T_Unqualified>().convert(value);
    }

    return value;
  }
} // Rice
