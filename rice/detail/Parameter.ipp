namespace Rice::detail
{
  // -----------  ParameterAbstract ----------------
  inline ParameterAbstract::ParameterAbstract(std::unique_ptr<Arg>&& arg) : arg_(std::move(arg))
  {
  }

  inline ParameterAbstract::ParameterAbstract(const ParameterAbstract& other)
  {
    this->arg_ = std::make_unique<Arg>(*other.arg_);
  }

  inline Arg* ParameterAbstract::arg()
  {
    return this->arg_.get();
  }

  // -----------  Parameter ----------------
  template<typename T>
  inline Parameter<T>::Parameter(std::unique_ptr<Arg>&& arg) : ParameterAbstract(std::move(arg)), 
    fromRuby_(this->arg()), toRuby_(this->arg())
  {
  }

  template<typename T>
  inline Convertible Parameter<T>::matches(std::optional<VALUE>& valueOpt)
  {
    Convertible result = Convertible::None;

    // Is a VALUE being passed directly to C++ ?
    if (valueOpt.has_value())
    {
      VALUE value = valueOpt.value();
      if (this->arg()->isValue())
      {
        result = Convertible::Exact;
      }
      // If index is less than argc then check with FromRuby if the VALUE is convertible
      // to C++.
      else
      {
        result = this->fromRuby_.is_convertible(value);

        // If this is an exact match check if the const-ness of the value and the parameter match.
        // One caveat - procs are also RUBY_T_DATA so don't check if this is a function type
        if (result == Convertible::Exact && rb_type(value) == RUBY_T_DATA && !std::is_function_v<std::remove_pointer_t<T>>)
        {
          // Check the constness of the Ruby wrapped value and the parameter
          WrapperBase* wrapper = getWrapper(value);

          // Do not send a const value to a non-const parameter
          if (wrapper->isConst() && !is_const_any_v<T>)
          {
            result = Convertible::None;
          }
          // It is ok to send a non-const value to a const parameter but
          // prefer non-const to non-const by slighly decreasing the convertible value
          else if (!wrapper->isConst() && is_const_any_v<T>)
          {
            result = Convertible::Const;
          }
        }
      }
    }
    // Last check if a default value has been set
    else if (this->arg()->hasDefaultValue())
    {
      result = Convertible::Exact;
    }

    return result;
  }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)  // unreachable code
#endif

  template<typename T>
  inline T Parameter<T>::convertToNative(std::optional<VALUE>& valueOpt)
  {
    /* In general the compiler will convert T to const T, but that does not work for converting
       T** to const T** (see see https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion)
       which comes up in the OpenCV bindings.

       An alternative solution is updating From_Ruby#convert to become a templated function that specifies
       the return type. That works but requires a lot more code changes for this one case and is not
       backwards compatible. */

    if constexpr (is_pointer_pointer_v<T> && !std::is_convertible_v<remove_cv_recursive_t<T>, T>)
    {
      return (T)this->fromRuby_.convert(valueOpt.value());
    }
    else if (valueOpt.has_value())
    {
      return this->fromRuby_.convert(valueOpt.value());
    }
    // Remember std::is_copy_constructible_v<std::vector<std::unique_ptr<T>>>> returns true. Sigh.
    // So special case vector handling
    else if constexpr (detail::is_std_vector_v<detail::intrinsic_type<T>>)
    {
      if constexpr (std::is_copy_constructible_v<typename detail::intrinsic_type<T>::value_type>)
      {
        if (this->arg()->hasDefaultValue())
        {
          return this->arg()->template defaultValue<T>();
        }
      }
    }
    else if constexpr (std::is_copy_constructible_v<T>)
    {
      if (this->arg()->hasDefaultValue())
      {
        return this->arg()->template defaultValue<T>();
      }
    }

    // This can be unreachable code
    throw std::invalid_argument("Could not convert Ruby value");
  }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

  template<typename T>
  inline VALUE Parameter<T>::convertToRuby(T object)
  {
    return this->toRuby_.convert(object);
  }

  template<typename T>
  inline std::string Parameter<T>::cppTypeName()
  {
    detail::TypeIndexParser typeIndexParser(typeid(T), std::is_fundamental_v<detail::intrinsic_type<T>>);
    return typeIndexParser.simplifiedName();
  }

  template<typename T>
  inline VALUE Parameter<T>::klass()
  {
    TypeMapper<T> typeMapper;
    return typeMapper.rubyKlass();
  }
}
