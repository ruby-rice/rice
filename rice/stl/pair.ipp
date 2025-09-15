#include <utility>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class PairHelper
    {
    public:
      PairHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->define_constructors();
        this->define_attributes();
        this->define_to_s();
      }

    private:
      void define_constructors()
      {
        klass_.define_constructor(Constructor<T>())
              .define_constructor(Constructor<T, typename T::first_type&, typename T::second_type&>());
      
        if constexpr (std::is_copy_constructible_v<typename T::first_type> && std::is_copy_constructible_v<typename T::second_type>)
        {
          klass_.define_constructor(Constructor<T, const T&>());
        }
      }

      void define_attributes()
      {
        // Access methods
        if constexpr (std::is_const_v<std::remove_reference_t<std::remove_pointer_t<typename T::first_type>>>)
        {
          klass_.define_attr("first", &T::first, Rice::AttrAccess::Read);
        }
        else
        {
          klass_.define_attr("first", &T::first, Rice::AttrAccess::ReadWrite);
        }

        if constexpr (std::is_const_v<std::remove_reference_t<std::remove_pointer_t<typename T::second_type>>>)
        {
          klass_.define_attr("second", &T::second, Rice::AttrAccess::Read);
        }
        else
        {
          klass_.define_attr("second", &T::second, Rice::AttrAccess::ReadWrite);
        }
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<typename T::first_type> && detail::is_ostreamable_v<typename T::second_type>)
        {
          klass_.define_method("to_s", [](const T& pair)
            {
              std::stringstream stream;
              stream << "[" << pair.first << ", " << pair.second << "]";
              return stream.str();
            });
        }
        else
        {
          klass_.define_method("to_s", [](const T& pair)
            {
              return "[Not printable]";
            });
        }
      }

      private:
        Data_Type<T> klass_;
    };
  } // namespace

  template<typename T1, typename T2>
  Data_Type<std::pair<T1, T2>> define_pair(std::string klassName)
  {
    using Pair_T = std::pair<T1, T2>;
    using Data_Type_T = Data_Type<Pair_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<Pair_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<Pair_T>>(rb_mStd, id);
    stl::PairHelper helper(result);
    return result;
  }

  namespace detail
  {
    template<typename T1, typename T2>
    struct Type<std::pair<T1, T2>>
    {
      static bool verify()
      {
        detail::verifyType<T1>();
        detail::verifyType<T2>();

        if (!Data_Type<std::pair<T1, T2>>::is_defined())
        {
          define_pair<T1, T2>();
        }

        return true;
      }
    };
  }
}
