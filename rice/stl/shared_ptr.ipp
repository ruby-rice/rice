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
      detail::TypeMapper<SharedPtr_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<SharedPtr_T>(rb_mStd, id);

    // std::shared_ptr<void> cannot be constructed from void* because void is incomplete
    // and the deleter cannot be determined. So skip the constructor for void.
    // std::shared_ptr<T[]> (array types) also skip constructor - arrays need special handling.
    if constexpr (!std::is_void_v<T> && !std::is_array_v<T>)
    {
      result.define_constructor(Constructor<SharedPtr_T, typename SharedPtr_T::element_type*>(), Arg("value").takeOwnership());
    }

    result.
      define_method("get", &SharedPtr_T::get).
      define_method("use_count", &SharedPtr_T::use_count).
      define_method("empty?", &SharedPtr_T::operator bool);

    return result;
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
      if constexpr (std::is_fundamental_v<T>)
      {
        Type<Pointer<T>>::verify();
        Type<Buffer<T>>::verify();
      }
      else
      {
        if (!Type<intrinsic_type<T>>::verify())
        {
          return false;
        }
      }

      define_shared_ptr<T>();

      return true;
    }
  };

  // Specialization for array types std::shared_ptr<T[]>
  template<typename T>
  struct Type<std::shared_ptr<T[]>>
  {
    static bool verify()
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        Type<Pointer<T>>::verify();
        Type<Buffer<T>>::verify();
      }
      else
      {
        if (!Type<intrinsic_type<T>>::verify())
        {
          return false;
        }
      }

      define_shared_ptr<T[]>();

      return true;
    }
  };
}
