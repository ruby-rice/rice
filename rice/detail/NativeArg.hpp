#ifndef Rice__detail__native_arg__hpp_
#define Rice__detail__native_arg__hpp_

#include <type_traits>

#include "ruby.hpp"

namespace Rice
{
  namespace detail
  {
    // Helper to get the base_type of T - without pointer, reference, const or volatile
    template<typename T>
    using base_type = std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;

    // Helper to determine if we have to keep a local copy of a value converted from Ruby.
    // This is needed if function takes a reference or a pointer to a fundamental type
    template <typename T, typename = void>
    struct must_copy : public std::false_type {};

    template <typename T>
    struct must_copy<T, std::enable_if_t<std::is_fundamental_v<base_type<T>> ||
      std::is_same_v<std::string, base_type<T>>>> : public std::true_type {};

    template <typename T>
    constexpr bool must_copy_v = must_copy<T>::value;

    // Primary Template for NativeArg
    template <typename T, typename Enable = void>
    class NativeArg;

    // NativeArg implementation that works on fundamental types. It keeps a copy of the
    // converted value so that a reference or a pointer to the value can be passed to 
    // the native function.
    template <typename T>
    class NativeArg<T, typename std::enable_if_t<must_copy_v<T>>>
    {
    public:
      using Base_T = std::decay_t<std::remove_pointer_t<T>>;

      T nativeValue(VALUE value)
      {
        this->native_ = From_Ruby<Base_T>::convert(value);

        if constexpr (std::is_pointer_v<T>)
        {
          return &this->native_;
        }
        else
        {
          return this->native_;
        }
      }

    private:
      Base_T native_;
    };

    // NativeArg implementation that works on all other types. The primary use is for 
    // pointers wrapped by Data_Object where there is no reason to store a local copy.
    // It is also used for converting to various Rice C++ wrappers such as Rice::Hash,
    // Rice::String, etc.
    template <typename T>
    class NativeArg<T, typename std::enable_if_t<!must_copy_v<T>>>
    {
    public:
      T nativeValue(VALUE value)
      {
        return From_Ruby<T>::convert(value);
      }
    };

  } // detail
} // Rice

#endif // Rice__detail__native_arg__hpp_
