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
        this->define_constructor();
        this->define_copyable_methods();
        this->define_access_methods();
        this->define_modify_methods();
        this->define_to_s();
      }

    private:
      void define_constructor()
      {
        klass_.define_constructor(Constructor<T, typename T::first_type&, typename T::second_type&>());
      }

      void define_copyable_methods()
      {
        if constexpr (std::is_copy_constructible_v<typename T::first_type> && std::is_copy_constructible_v<typename T::second_type>)
        {
          klass_.define_method("copy", [](T& pair) -> T
            {
              return pair;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& pair) -> T
            {
              throw std::runtime_error("Cannot copy pair with non-copy constructible types");
              return pair;
            });
        }
      }

      void define_access_methods()
      {
        // Access methods
        klass_.define_method("first", [](T& pair) -> typename T::first_type&
          {
            return pair.first;
          })
        .define_method("second", [](T& pair) -> typename T::second_type&
          {
            return pair.second;
          });
      }

      void define_modify_methods()
      {
        // Access methods
        klass_.define_method("first=", [](T& pair, typename T::first_type& value) -> typename T::first_type&
        {
          if constexpr (std::is_const_v<std::remove_reference_t<std::remove_pointer_t<typename T::first_type>>>)
          {
            throw std::runtime_error("Cannot set pair.first since it is a constant");
          }
          else
          {
            pair.first = value;
            return pair.first;
          }
        })
        .define_method("second=", [](T& pair, typename T::second_type& value) -> typename T::second_type&
        {
          if constexpr (std::is_const_v<std::remove_reference_t<std::remove_pointer_t<typename T::second_type>>>)
          {
            throw std::runtime_error("Cannot set pair.second since it is a constant");
          }
          else
          {
            pair.second = value;
            return pair.second;
          }
        });
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

  template<typename T>
  Data_Type<T> define_pair_under(Object parent, std::string name)
  {
    if (Data_Type<T>::check_defined(name, parent))
    {
      return Data_Type<T>();
    }

    Data_Type<T> result = define_class_under<detail::intrinsic_type<T>>(parent, name.c_str());
    stl::PairHelper helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_pair(std::string name)
  {
    if (Data_Type<T>::check_defined(name))
    {
      return Data_Type<T>();
    }

    Data_Type<T> result = define_class<detail::intrinsic_type<T>>(name.c_str());
    stl::PairHelper<T> helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_pair_auto()
  {
    Module rb_mStd = define_module("Std");
    std::string name = detail::typeName(typeid(T));
    std::string klassName = detail::makeClassName(name);
    return define_pair_under<T>(rb_mStd, klassName);
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
          define_pair_auto<std::pair<T1, T2>>();
        }

        return true;
      }
    };
  }
}
