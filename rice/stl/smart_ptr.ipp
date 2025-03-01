#include "../detail/to_ruby.hpp"
#include "../detail/Registries.hpp"

#include <assert.h>
#include <memory>

namespace Rice::detail
{
  // ---- WrapperSmartPointer ------
  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  inline WrapperSmartPointer<SmartPointer_T, Arg_Ts...>::WrapperSmartPointer(SmartPointer_T<Arg_Ts...> data) 
    : data_(std::move(data))
  {
  }

  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  inline WrapperSmartPointer<SmartPointer_T, Arg_Ts...>::~WrapperSmartPointer()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  inline void* WrapperSmartPointer<SmartPointer_T, Arg_Ts...>::get()
  {
    return (void*)this->data_.get();
  }

  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  inline SmartPointer_T<Arg_Ts...>& WrapperSmartPointer<SmartPointer_T, Arg_Ts...>::data()
  {
    return data_;
  }

  // ---- unique_ptr ------
  template <typename T>
  class To_Ruby<std::unique_ptr<T>>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;

    VALUE convert(std::unique_ptr<T>& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }
  };

  template <typename T>
  class To_Ruby<std::unique_ptr<T>&>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;

    VALUE convert(std::unique_ptr<T>& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }
  };

  template <typename T>
  class From_Ruby<std::unique_ptr<T>>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;

    Wrapper_T* is_same_smart_ptr(VALUE value)
    {
      WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
      return dynamic_cast<Wrapper_T*>(wrapper);
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
      Wrapper_T* smartWrapper = is_same_smart_ptr(value);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return std::move(smartWrapper->data());
    }
  };

  template <typename T>
  class From_Ruby<std::unique_ptr<T>&>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;

    Wrapper_T* is_same_smart_ptr(VALUE value)
    {
      WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
      return dynamic_cast<Wrapper_T*>(wrapper);
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
      Wrapper_T* smartWrapper = is_same_smart_ptr(value);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return smartWrapper->data();
    }
  };

  template<typename T>
  struct Type<std::unique_ptr<T>>
  {
    static bool verify()
    {
      return Type<T>::verify();
    }
  };

  // ----- shared_ptr -------------
  template <typename T>
  class To_Ruby<std::shared_ptr<T>>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;

    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        rb_data_type_t* ruby_data_type = RubyType<T>::ruby_data_type();
        VALUE klass = RubyType<T>::klass();
        return detail::wrap<std::shared_ptr<T>, Wrapper_T>(klass, ruby_data_type, data, true);
      }
      else
      {
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
        return detail::wrap<std::shared_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
      }
    }
  };

  template <typename T>
  class From_Ruby<std::shared_ptr<T>>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;

    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }

    Wrapper_T* is_same_smart_ptr(VALUE value)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        WrapperBase* wrapper = detail::getWrapper(value, RubyType<T>::ruby_data_type());
        return dynamic_cast<Wrapper_T*>(wrapper);
      }
      else
      {
        WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
        return dynamic_cast<Wrapper_T*>(wrapper);
      }
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

    std::shared_ptr<T> convert(VALUE value)
    {
      if(value == Qnil && this->arg_ && this->arg_->hasDefaultValue()) {
        return this->arg_->template defaultValue<std::shared_ptr<T>>();
      }

      Wrapper_T* smartWrapper = is_same_smart_ptr(value);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
          throw std::runtime_error(message.c_str());
      }
      return smartWrapper->data();
    }
  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<std::shared_ptr<T>&>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;

    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        rb_data_type_t* ruby_data_type = RubyType<T>::ruby_data_type();
        VALUE klass = RubyType<T>::klass();
        return detail::wrap<std::shared_ptr<T>, Wrapper_T>(klass, ruby_data_type, data, true);
      }
      else
      {
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
        return detail::wrap<std::shared_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
      }
    }
  };

  template <typename T>
  class From_Ruby<std::shared_ptr<T>&>
  {
  public:
    using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;

    From_Ruby() = default;

    explicit From_Ruby(Arg * arg) : arg_(arg)
    {
    }

    Wrapper_T* is_same_smart_ptr(VALUE value)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        WrapperBase* wrapper = detail::getWrapper(value, RubyType<T>::ruby_data_type());
        return dynamic_cast<Wrapper_T*>(wrapper);
      }
      else
      {
        WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
        return dynamic_cast<Wrapper_T*>(wrapper);
      }
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

    std::shared_ptr<T>& convert(VALUE value)
    {
      if(value == Qnil && this->arg_ && this->arg_->hasDefaultValue()) {
        return this->arg_->template defaultValue<std::shared_ptr<T>>();
      }

      Wrapper_T* smartWrapper = is_same_smart_ptr(value);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
          throw std::runtime_error(message.c_str());
      }
      return smartWrapper->data();
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<typename T>
  struct Type<std::shared_ptr<T>>
  {
    static bool verify()
    {
      return Type<T>::verify();
    }
  };
}
