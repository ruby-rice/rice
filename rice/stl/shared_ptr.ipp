#include <memory>

// --------- Enable creation of std::shared_ptr from Ruby ---------
namespace Rice
{
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName)
  {
    using SharedPtr_T = std::shared_ptr<T>;
    using Data_Type_T = Data_Type<SharedPtr_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(SharedPtr_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<SharedPtr_T>>(rb_mStd, id).
      define_constructor(Constructor<SharedPtr_T, typename SharedPtr_T::element_type*>(), Arg("value").takeOwnership());

    return result;
  }
}

// --------- Wrapper ---------
namespace Rice::detail
{
  template<typename T>
  inline Wrapper<std::shared_ptr<T>>::Wrapper(const std::shared_ptr<T>& data)
    : data_(data)
  {
  }

  template<typename T>
  inline Wrapper<std::shared_ptr<T>>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template<typename T>
  inline void* Wrapper<std::shared_ptr<T>>::get()
  {
    return (void*)this->data_.get();
  }

  template<typename T>
  inline std::shared_ptr<T>& Wrapper<std::shared_ptr<T>>::data()
  {
    return data_;
  }
}

// --------- Type/To_Ruby/From_Ruby ---------
namespace Rice::detail
{
  template<typename T>
  struct Type<std::shared_ptr<T>>
  {
    static bool verify()
    {
      return Type<T>::verify();
    }
  };

  template <typename T>
  class To_Ruby<std::shared_ptr<T>>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arv)
    {
    }

    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return detail::wrap(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
      else
      {
        return detail::wrap<std::shared_ptr<T>>(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
    }

    VALUE convert(std::shared_ptr<T>&& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return detail::wrap(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
      else
      {
        return detail::wrap<std::shared_ptr<T>>(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
    }
  };

  template <typename T>
  class From_Ruby<std::shared_ptr<T>>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::shared_ptr<T> convert(VALUE value)
    {
      // Get the wrapper
      WrapperBase* wrapperBase = detail::getWrapper(value);

      // Was this shared_ptr created by the user from Ruby? If so it will
      // be wrapped as a pointer, std::shared_ptr<T>*. In the case just
      // return the shared pointer
      if (dynamic_cast<Wrapper<std::shared_ptr<T>*>*>(wrapperBase))
      {
        // Use unwrap to validate the underlying wrapper is the correct type
        std::shared_ptr<T>* ptr = unwrap<std::shared_ptr<T>>(value, Data_Type<std::shared_ptr<T>>::ruby_data_type(), false);
        return *ptr;
      }
      else if constexpr (std::is_fundamental_v<T>)
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
      else
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
    }
  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<std::shared_ptr<T>&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg)
    {
    }

    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return detail::wrap(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
      else
      {
        return detail::wrap<std::shared_ptr<T>>(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
    }
  };

  template <typename T>
  class From_Ruby<std::shared_ptr<T>&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::shared_ptr<T>& convert(VALUE value)
    {
      // Get the wrapper
      WrapperBase* wrapperBase = detail::getWrapper(value);

      // Was this shared_ptr created by the user from Ruby? If so it will
      // be wrapped as a pointer, std::shared_ptr<T>*. In the case just
      // return the shared pointer
      if (dynamic_cast<Wrapper<std::shared_ptr<T>*>*>(wrapperBase))
      {
        // Use unwrap to validate the underlying wrapper is the correct type
        std::shared_ptr<T>* ptr = unwrap<std::shared_ptr<T>>(value, Data_Type<std::shared_ptr<T>>::ruby_data_type(), false);
        return *ptr;
      }
      else if constexpr (std::is_fundamental_v<T>)
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
      else
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
    }

  private:
    Arg* arg_ = nullptr;
  };
}
