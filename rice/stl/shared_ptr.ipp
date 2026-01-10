#include <memory>

namespace Rice
{
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName)
  {
    using SharedPtr_T = std::shared_ptr<T>;
    using Data_Type_T = Data_Type<SharedPtr_T>;

    if (klassName.empty())
    {
      detail::TypeDetail<SharedPtr_T> typeDetail;
      klassName = typeDetail.rubyName();
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<SharedPtr_T>>(rb_mStd, id).
      define_method("get", &SharedPtr_T::get).
      define_method("swap", &SharedPtr_T::swap, Arg("r")).
      define_method("use_count", &SharedPtr_T::use_count).
      define_method("empty?", [](SharedPtr_T& self)->bool
      {
        return !self;
      });

    if constexpr (!std::is_void_v<T>)
    {
      result.define_constructor(Constructor<SharedPtr_T, typename SharedPtr_T::element_type*>(), Arg("value").takeOwnership());
    }

    // Setup delegation to forward T's methods via get (only for non-fundamental, non-void types)
    if constexpr (!std::is_void_v<T> && !std::is_fundamental_v<T>)
    {
      detail::define_forwarding(result.klass(), Data_Type<T>::klass());
    }

    return result;
  }
}

// --------- Wrapper ---------
namespace Rice::detail
{
  template<typename T>
  Wrapper<std::shared_ptr<T>>::Wrapper(rb_data_type_t* rb_data_type, const std::shared_ptr<T>& data)
    : WrapperBase(rb_data_type), data_(data)
  {
    using Intrinsic_T = intrinsic_type<T>;

    if constexpr (std::is_fundamental_v<Intrinsic_T>)
    {
      inner_rb_data_type_ = Data_Type<Pointer<Intrinsic_T>>::ruby_data_type();
    }
    else
    {
      inner_rb_data_type_ = Data_Type<Intrinsic_T>::ruby_data_type();
    }
  }

  template<typename T>
  Wrapper<std::shared_ptr<T>>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get(this->rb_data_type_));
  }

  template<typename T>
  void* Wrapper<std::shared_ptr<T>>::get(rb_data_type_t* requestedType)
  {
    if (rb_typeddata_inherited_p(this->rb_data_type_, requestedType))
    {
      return &this->data_;
    }
    else if (rb_typeddata_inherited_p(this->inner_rb_data_type_, requestedType))
    {
      return this->data_.get();
    }
    else
    {
      throw Exception(rb_eTypeError, "wrong argument type (expected %s)",
          requestedType->wrap_struct_name);
    }
  }
}

// --------- Type ---------
namespace Rice::detail
{
  template<typename T>
  struct Type<std::shared_ptr<T>>
  {
    static bool verify()
    {
      bool result = true;
      if constexpr (std::is_fundamental_v<T>)
      {
        result = result && Type<Pointer<T>>::verify();
      }
      else
      {
        result = result && Type<T>::verify();
      }

      if (result)
      {
        define_shared_ptr<T>();
      }

      return result;
    }
  };

#ifdef __GLIBCXX__
  // libstdc++ implementation detail: std::shared_ptr inherits from
  // std::__shared_ptr<T, _Lock_policy>. Methods like swap() expose this
  // internal type through their parameter signatures.
  template<typename T, __gnu_cxx::_Lock_policy Policy>
  struct Type<std::__shared_ptr<T, Policy>>
  {
    static bool verify()
    {
      return true;
    }
  };
#endif
}
