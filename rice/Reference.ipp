namespace Rice
{
  template<typename T>
  inline Reference<T>::Reference() : data_{}
  {
  }

  template<typename T>
  inline Reference<T>::Reference(T& data) : data_(data)
  {
  }

  template<typename T>
  inline Reference<T>::Reference(VALUE value) : data_(detail::FromRubyFundamental<T>::convert(value))
  {
  }

  template<typename T>
  inline T& Reference<T>::get()
  {
    return data_;
  }

  // Specialization implementations - only one is compiled per platform
#if SIZEOF_LONG == SIZEOF_VOIDP
  // VALUE is unsigned long on Linux/macOS
  inline Reference<unsigned long>::Reference() : data_{}
  {
  }

  inline Reference<unsigned long>::Reference(unsigned long value, bool isValue) :
    data_(isValue ? detail::FromRubyFundamental<unsigned long>::convert(value) : value)
  {
  }

  inline unsigned long& Reference<unsigned long>::get()
  {
    return data_;
  }
#else
  // VALUE is unsigned long long on Windows x64
  inline Reference<unsigned long long>::Reference() : data_{}
  {
  }

  inline Reference<unsigned long long>::Reference(unsigned long long value, bool isValue) :
    data_(isValue ? detail::FromRubyFundamental<unsigned long long>::convert(value) : value)
  {
  }

  inline unsigned long long& Reference<unsigned long long>::get()
  {
    return data_;
  }
#endif

  template<typename T>
  inline Data_Type<Reference<T>> define_reference(std::string klassName)
  {
    using Reference_T = Reference<T>;
    using Data_Type_T = Data_Type<Reference_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<Reference_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mRice = define_module("Rice");

    if (Data_Type_T::check_defined(klassName, rb_mRice))
    {
      return Data_Type_T();
    }

    Data_Type<Reference<T>> result = define_class_under<Reference_T>(rb_mRice, klassName).
      define_constructor(Constructor<Reference_T, T>()).
      define_method("value", &Reference_T::get);

    return result;
  }
}

namespace Rice::detail
{
  template<typename T>
  struct Type<Reference<T>>
  {
    static bool verify()
    {
      detail::verifyType<T>();
      define_reference<T>();
      return true;
    }
  };
}
