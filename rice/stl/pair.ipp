#include "../detail/rice_traits.hpp"
#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"
#include "../Data_Type.hpp"

#include <sstream>
#include <stdexcept>
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
          klass_.define_method("copy", [](T& self) -> T
            {
              return self;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& self) -> T
            {
              throw std::runtime_error("Cannot copy pair with non-copy constructible types");
              return self;
            });
        }
      }

      void define_access_methods()
      {
        // Access methods
        klass_.define_method("first", [](T& self) -> typename T::first_type&
          {
            return self.first;
          })
        .define_method("second", [](T& self) -> typename T::second_type&
          {
            return self.second;
          });
      }

      void define_modify_methods()
      {
        // Access methods
        klass_.define_method("first=", [](T& self, typename T::first_type& value) -> typename T::first_type&
          {
            self.first = value;
            return self.first;
          })
        .define_method("second=", [](T& self, typename T::second_type& value) -> typename T::second_type&
          {
            self.second = value;
            return self.second;
          });
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<typename T::first_type> && detail::is_ostreamable_v<typename T::second_type>)
        {
          klass_.define_method("to_s", [](const T& self)
            {
              std::stringstream stream;
              stream << "[" << self.first << ", " << self.second << "]";
              return stream.str();
            });
        }
        else
        {
          klass_.define_method("to_s", [](const T& self)
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
  Data_Type<T> define_pair_under(Object module, std::string name)
  {
    if (Data_Type<T>::isDefined)
    {
      return Data_Type<T>(Data_Type<T>());
    }

    Data_Type<T> result = define_class_under<detail::intrinsic_type<T>>(module, name.c_str());
    stl::PairHelper helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_pair(std::string name)
  {
    if (Data_Type<T>::isDefined)
    {
      return Data_Type<T>(Data_Type<T>());
    }

    Data_Type<T> result = define_class<detail::intrinsic_type<T>>(name.c_str());
    stl::PairHelper<T> helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_pair_auto()
  {
    std::string klassName = detail::makeClassName(typeid(T));
    Module rb_mRice = define_module("Rice");
    Module rb_mpair = define_module_under(rb_mRice, "Std");
    return define_pair_under<T>(rb_mpair, klassName);
  }
   
  namespace detail
  {
    template<typename T1, typename T2>
    struct Type<std::pair<T1, T2>>
    {
      constexpr static void verify()
      {
        if (!Data_Type<std::pair<T1, T2>>::isDefined)
        {
          define_pair_auto<std::pair<T1, T2>>();
        }
      }
    };
  }
}
