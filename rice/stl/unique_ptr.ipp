#include <memory>

namespace Rice::detail
{
  template<typename T>
  inline Wrapper<std::unique_ptr<T>>::Wrapper(std::unique_ptr<T>&& data)
    : data_(std::move(data))
  {
  }

  template<typename T>
  inline Wrapper<std::unique_ptr<T>>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template<typename T>
  inline void* Wrapper<std::unique_ptr<T>>::get()
  {
    return (void*)this->data_.get();
  }

  template<typename T>
  inline std::unique_ptr<T>& Wrapper<std::unique_ptr<T>>::data()
  {
    return data_;
  }

  template <typename T>
  class To_Ruby<std::unique_ptr<T>>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(std::unique_ptr<T>& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

    VALUE convert(std::unique_ptr<T>&& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<std::unique_ptr<T>&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(std::unique_ptr<T>& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class From_Ruby<std::unique_ptr<T>>
  {
  public:
    Wrapper<std::unique_ptr<T>>* is_same_smart_ptr(VALUE value)
    {
      WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
      return dynamic_cast<Wrapper<std::unique_ptr<T>>*>(wrapper);
    }

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      if (!is_same_smart_ptr(value))
        return Convertible::None;

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::unique_ptr<T> convert(VALUE value)
    {
      Wrapper<std::unique_ptr<T>>* wrapper = is_same_smart_ptr(value);
      if (!wrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return std::move(wrapper->data());
    }

  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class From_Ruby<std::unique_ptr<T>&>
  {
  public:
    Wrapper<std::unique_ptr<T>>* is_same_smart_ptr(VALUE value)
    {
      WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
      return dynamic_cast<Wrapper<std::unique_ptr<T>>*>(wrapper);
    }

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      if (!is_same_smart_ptr(value))
        return Convertible::None;

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::unique_ptr<T>& convert(VALUE value)
    {
      Wrapper<std::unique_ptr<T>>* wrapper = is_same_smart_ptr(value);
      if (!wrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return wrapper->data();
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  struct Type<std::unique_ptr<T>>
  {
    static bool verify()
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return Type<Pointer<T>>::verify();
        return Type<Buffer<T>>::verify();
      }
      else
      {
        return Type<T>::verify();
      }
    }

    static VALUE rubyKlass()
    {
      TypeMapper<T> typeMapper;
      return typeMapper.rubyKlass();
    }
  };
}
