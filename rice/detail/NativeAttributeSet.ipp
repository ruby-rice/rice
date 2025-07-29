#include <array>
#include <algorithm>


namespace Rice::detail
{
  template<typename Attribute_T>
  void NativeAttributeSet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute)
  {
    // Create a NativeAttributeSet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute));
    std::unique_ptr<Native> native(nativeAttribute);

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
  inline Resolved NativeAttributeSet<Attribute_T>::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    if (argc == 1)
      return Resolved{ Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeSet<Attribute_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    if (argc != 1)
    {
      throw std::runtime_error("Incorrect number of parameters for setting attribute. Attribute: " + this->name_);
    }

    VALUE value = argv[0];

    if constexpr (!std::is_null_pointer_v<Receiver_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);

      // Deal with pointers to pointes, see Parameter::convertToNative commment
      if constexpr (is_pointer_pointer_v<Attr_T> && !std::is_convertible_v<remove_cv_recursive_t<Attr_T>, Attr_T>)
      {
        nativeSelf->*attribute_ = (Attr_T)From_Ruby<T_Unqualified>().convert(value);
      }
      else
      {
        nativeSelf->*attribute_ = From_Ruby<T_Unqualified>().convert(value);
      }
    }
    else
    {
      *attribute_ = From_Ruby<T_Unqualified>().convert(value);
    }

    return value;
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeSet<Attribute_T>::toString()
  {
    return "";
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeSet<Attribute_T>::name()
  {
    return this->name_;
  }

  template<typename Attribute_T>
  inline NativeKind NativeAttributeSet<Attribute_T>::kind()
  {
    return NativeKind::AttributeWriter;
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeSet<Attribute_T>::rubyReturnType()
  {
    if constexpr (std::is_fundamental_v<Attr_T>)
    {
      return RubyType<detail::remove_cv_recursive_t<Attr_T>>::name;
    }
    else
    {
      return rubyClassName<Attr_T>();
    }
  }
} // Rice
