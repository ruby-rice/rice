#include "../detail/to_ruby.hpp"
#include "../detail/Registries.hpp"

#include <assert.h>
#include <memory>

namespace Rice::detail
{
  template<typename T>
  inline Wrapper<std::shared_ptr<T>>::Wrapper(std::shared_ptr<T>& data)
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

  template <typename T>
  class To_Ruby<std::shared_ptr<T>>
  {
  public:
    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        rb_data_type_t* ruby_data_type = RubyType<T>::ruby_data_type();
        VALUE klass = RubyType<T>::klass();
        return detail::wrap<std::shared_ptr<T>>(klass, ruby_data_type, data, true);
      }
      else
      {
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
        return detail::wrap<std::shared_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
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

    Wrapper<std::shared_ptr<T>>* is_same_smart_ptr(VALUE value)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        WrapperBase* wrapper = detail::getWrapper(value, RubyType<T>::ruby_data_type());
        return dynamic_cast<Wrapper<std::shared_ptr<T>>*>(wrapper);
      }
      else
      {
        WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
        return dynamic_cast<Wrapper<std::shared_ptr<T>>*>(wrapper);
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

      Wrapper<std::shared_ptr<T>>* wrapper = is_same_smart_ptr(value);
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

  template <typename T>
  class To_Ruby<std::shared_ptr<T>&>
  {
  public:
    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        rb_data_type_t* ruby_data_type = RubyType<T>::ruby_data_type();
        VALUE klass = RubyType<T>::klass();
        return detail::wrap<std::shared_ptr<T>>(klass, ruby_data_type, data, true);
      }
      else
      {
        std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
        return detail::wrap<std::shared_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
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

    Wrapper<std::shared_ptr<T>>* is_same_smart_ptr(VALUE value)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        WrapperBase* wrapper = detail::getWrapper(value, RubyType<T>::ruby_data_type());
        return dynamic_cast<Wrapper<std::shared_ptr<T>>*>(wrapper);
      }
      else
      {
        WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
        return dynamic_cast<Wrapper<std::shared_ptr<T>>*>(wrapper);
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

      Wrapper<std::shared_ptr<T>>* wrapper = is_same_smart_ptr(value);
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
  struct Type<std::shared_ptr<T>>
  {
    static bool verify()
    {
      return Type<T>::verify();
    }
  };
}
