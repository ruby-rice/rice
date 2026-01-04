#include <memory>

namespace Rice
{
  template<typename T>
  Data_Type<std::unique_ptr<T>> define_unique_ptr(std::string klassName)
  {
    using UniquePtr_T = std::unique_ptr<T>;
    using Data_Type_T = Data_Type<UniquePtr_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<UniquePtr_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<UniquePtr_T>>(rb_mStd, id).
      define_method("get", &UniquePtr_T::get).
      define_method("release", &UniquePtr_T::release).
      define_method("reset", &UniquePtr_T::reset, Arg("ptr")).
      define_method("swap", &UniquePtr_T::swap, Arg("other")).
      define_method("empty?", [](UniquePtr_T& self)->bool
      {
        return !self;
      });

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
  Wrapper<std::unique_ptr<T>>::Wrapper(rb_data_type_t* rb_data_type, std::unique_ptr<T>&& data)
    : WrapperBase(rb_data_type), data_(std::move(data))
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
  Wrapper<std::unique_ptr<T>>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get(this->rb_data_type_));
  }

  template<typename T>
  void* Wrapper<std::unique_ptr<T>>::get(rb_data_type_t* requestedType)
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
  struct Type<std::unique_ptr<T>>
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
        define_unique_ptr<T>();
      }

      return result;
    }
  };

  // --------- From_Ruby ---------
  template <typename T>
  class From_Ruby<std::unique_ptr<T>>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    double is_convertible(VALUE value)
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

    std::unique_ptr<T> convert(VALUE value)
    {
      std::unique_ptr<T>* result = detail::unwrap<std::unique_ptr<T>>(value, Data_Type<std::unique_ptr<T>>::ruby_data_type(), this->arg_ && this->arg_->isOwner());
      // The reason we need this overriden From_Ruby is to do this std::move.
      return std::move(*result);
    }

  private:
    Arg* arg_ = nullptr;
  };
}
