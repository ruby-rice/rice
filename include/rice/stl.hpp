#ifndef Rice__stl__hpp_
#define Rice__stl__hpp_


// =========   smart_ptr.hpp   =========


namespace Rice::detail
{
  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  class WrapperSmartPointer : public Wrapper
  {
  public:
    WrapperSmartPointer(SmartPointer_T<Arg_Ts...>& data);
    void* get() override;
    SmartPointer_T<Arg_Ts...>& data();

  private:
    SmartPointer_T<Arg_Ts...> data_;
  };
}


// ---------   smart_ptr.ipp   ---------

#include <assert.h>
#include <memory>

namespace Rice::detail
{
  // ---- WrapperSmartPointer ------
  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  inline WrapperSmartPointer<SmartPointer_T, Arg_Ts...>::WrapperSmartPointer(SmartPointer_T<Arg_Ts...>& data) 
    : data_(std::move(data))
  {
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
  struct To_Ruby<std::unique_ptr<T>>
  {
    static VALUE convert(std::unique_ptr<T>& data, bool takeOwnership = true)
    {
      assert(takeOwnership);
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::TypeRegistry::figureType<T>(*data);

      // Use custom wrapper type 
      using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;
      return detail::wrap<std::unique_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }
  };

  template <typename T>
  struct From_Ruby<std::unique_ptr<T>&>
  {
    static std::unique_ptr<T>& convert(VALUE value)
    {
      Wrapper* wrapper = detail::getWrapper(value, Data_Type<T>::rb_type());

      using Wrapper_T = WrapperSmartPointer<std::unique_ptr, T>;
      Wrapper_T* smartWrapper = dynamic_cast<Wrapper_T*>(wrapper);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return smartWrapper->data();
    }
  };

  // ----- shared_ptr -------------
  template <typename T>
  struct To_Ruby<std::shared_ptr<T>>
  {
    static VALUE convert(std::shared_ptr<T>& data, bool takeOwnership = true)
    {
      assert(takeOwnership);
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::TypeRegistry::figureType<T>(*data);

      // Use custom wrapper type 
      using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;
      return detail::wrap<std::shared_ptr<T>, Wrapper_T>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }
  };

  template <typename T>
  struct From_Ruby<std::shared_ptr<T>>
  {
    static std::shared_ptr<T> convert(VALUE value)
    {
      Wrapper* wrapper = detail::getWrapper(value, Data_Type<T>::rb_type());

      using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;
      Wrapper_T* smartWrapper = dynamic_cast<Wrapper_T*>(wrapper);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
          throw std::runtime_error(message.c_str());
      }
      return smartWrapper->data();
    }
  };

  template <typename T>
  struct From_Ruby<std::shared_ptr<T>&>
  {
    static std::shared_ptr<T>& convert(VALUE value)
    {
      Wrapper* wrapper = detail::getWrapper(value, Data_Type<T>::rb_type());

      using Wrapper_T = WrapperSmartPointer<std::shared_ptr, T>;
      Wrapper_T* smartWrapper = dynamic_cast<Wrapper_T*>(wrapper);
      if (!smartWrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
          throw std::runtime_error(message.c_str());
      }
      return smartWrapper->data();
    }
  };
}

// =========   vector.hpp   =========


// ---------   vector.ipp   ---------

namespace Rice::detail
{
  template<typename T>
  struct From_Ruby<std::vector<T>>
  {
    static std::vector<T> convert(VALUE x)
    {
      auto len = RARRAY_LEN(x);
      std::vector<T> vec;
      vec.reserve(len);
      for (long i = 0; i < len; i++) {
        vec.push_back(From_Ruby<T>::convert(rb_ary_entry(x, i)));
      }
      return vec;
    }
  };

  template<typename T>
  struct To_Ruby<std::vector<T>>
  {
    static VALUE convert(std::vector<T> const & x, bool takeOwnership = false)
    {
      auto a = rb_ary_new2(x.size());
      for (const auto& v : x) {
        rb_ary_push(a, To_Ruby<T>::convert(v, takeOwnership));
      }
      return a;
    }
  };
}


#endif // Rice__stl__hpp_
