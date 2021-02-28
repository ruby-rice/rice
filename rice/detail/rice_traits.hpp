#ifndef Rice__detail__traits__hpp_
#define Rice__detail__traits__hpp_

#include <string>
#include <type_traits>

namespace Rice
{
  namespace detail
  {
    // Get the base_type of T - without pointer, reference, const or volatile
    template<typename T>
    using intrinsic_type = typename std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;

    // Helper to determine if we have to keep a local copy of a value converted from Ruby.
    // This is needed for primitive types (numeric, char, bool and we include strings)
    template <typename T, typename = void>
    struct is_primitive : public std::false_type {};

    template <typename T>
    struct is_primitive<T, std::enable_if_t<std::is_fundamental_v<intrinsic_type<T>> ||
      std::is_same_v<std::string, intrinsic_type<T>>>> : public std::true_type {};

    template <typename T>
    constexpr bool is_primitive_v = is_primitive<T>::value;
  } // detail
} // Rice

#endif // Rice__detail__traits__hpp_
