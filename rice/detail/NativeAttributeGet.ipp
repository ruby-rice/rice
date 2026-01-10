#include <array>
#include <algorithm>

namespace Rice::detail
{
  template<typename Attribute_T>
  template<typename...Arg_Ts>
  void NativeAttributeGet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute, Arg_Ts&...args)
  {
    // Verify attribute type
    Native::verify_type<Attr_T, is_one_of_v<ReturnBuffer, Arg_Ts...>>();

    // Create return info
    std::unique_ptr<Return> returnInfo = Native::create_return<Arg_Ts...>(args...);

    // Create a NativeAttributeGet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute), std::move(returnInfo));
    std::unique_ptr<Native> native(nativeAttribute);

    detail::protect(rb_define_method, klass, name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry. Since attributes cannot be overridden, there is no need to set the
    // matches or calls function pointer. Instead Ruby can call the static call method defined on
    // this class (&NativeAttribute_T::get).
    Identifier identifier(name);
    detail::Registries::instance.natives.replace(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeGet<Attribute_T>::matches(std::map<std::string, VALUE>& values)
  {
    if (values.size() == 0)
      return Resolved{ Convertible::Exact, this };
    else
      return Resolved{ Convertible::None, this };
  }
  
  template<typename Attribute_T>
  NativeAttributeGet<Attribute_T>::NativeAttributeGet(VALUE klass, std::string name, Attribute_T attribute, std::unique_ptr<Return>&& returnInfo)
    : Native(name, std::move(returnInfo)),
      klass_(klass), attribute_(attribute)
  {
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::operator()(std::map<std::string, VALUE>&, VALUE self)
  {
    if constexpr (std::is_member_object_pointer_v<Attribute_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);

      if constexpr (std::is_fundamental_v<detail::intrinsic_type<To_Ruby_T>>)
      {
        return To_Ruby<To_Ruby_T>(this->returnInfo_.get()).convert(nativeSelf->*attribute_);
      }
      else if constexpr (std::is_array_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(this->returnInfo_.get()).convert(nativeSelf->*attribute_);
      }
      else if constexpr (std::is_pointer_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(this->returnInfo_.get()).convert(nativeSelf->*attribute_);
      }
      else
      {
        // If the attribute is an object return a reference to avoid a copy (and avoid issues with
        // attributes that are not assignable, copy constructible or move constructible)
        return To_Ruby<To_Ruby_T&>(this->returnInfo_.get()).convert(nativeSelf->*attribute_);
      }
    }
    else
    {
      if constexpr (std::is_fundamental_v<detail::intrinsic_type<To_Ruby_T>>)
      {
        return To_Ruby<To_Ruby_T>(this->returnInfo_.get()).convert(*attribute_);
      }
      else if constexpr (std::is_array_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(this->returnInfo_.get()).convert(*attribute_);
      }
      else if constexpr (std::is_pointer_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(this->returnInfo_.get()).convert(*attribute_);
      }
      else
      {
        // If the attribute is an object return a reference to avoid a copy (and avoid issues with
        // attributes that are not assignable, copy constructible or move constructible)
        return To_Ruby<To_Ruby_T&>(this->returnInfo_.get()).convert(*attribute_);
      }
    }
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeGet<Attribute_T>::toString()
  {
    return "";
  }

  template<typename Attribute_T>
  inline NativeKind NativeAttributeGet<Attribute_T>::kind()
  {
    return NativeKind::AttributeReader;
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::returnKlass()
  {
    // Check if an array is being returned
    bool isBuffer = dynamic_cast<ReturnBuffer*>(this->returnInfo_.get()) ? true : false;
    if (isBuffer)
    {
      TypeDetail<Pointer<detail::remove_cv_recursive_t<std::remove_pointer_t<Attr_T>>>> typeDetail;
      return typeDetail.rubyKlass();
    }
    else
    {
      TypeDetail<Attr_T> typeDetail;
      return typeDetail.rubyKlass();
    }
  }
}