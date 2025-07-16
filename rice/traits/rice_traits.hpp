#ifndef Rice__detail__traits__hpp_
#define Rice__detail__traits__hpp_

#include <ostream>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace Rice
{
  namespace detail
  {
    // Get the base_type of T - without pointer, reference, const or volatile. We call remove_pointer_t twice 
    // for T**
    template<typename T>
    using intrinsic_type = typename std::remove_cv_t<std::remove_pointer_t<std::remove_pointer_t<std::remove_reference_t<T>>>>;

    template<typename T>
    constexpr bool is_const_any_v = std::is_const_v<std::remove_pointer_t<std::remove_pointer_t<std::remove_reference_t<T>>>>;

    // Recursively remove const/volatile
    template<typename T>
    struct remove_cv_recursive
    {
      using type = T;
    };

    template<typename T>
    struct remove_cv_recursive<T const volatile>
    {
      using type = typename remove_cv_recursive<T>::type;
    };

    template<typename T>
    struct remove_cv_recursive<T volatile>
    {
      using type = typename remove_cv_recursive<T>::type;
    };

    template<typename T>
    struct remove_cv_recursive<T const>
    {
      using type = typename remove_cv_recursive<T>::type;
    };

    template<typename T>
    struct remove_cv_recursive<T&>
    {
      using type = typename remove_cv_recursive<T>::type&;
    };

    template<typename T>
    struct remove_cv_recursive<T&&>
    {
      using type = typename remove_cv_recursive<T>::type&&;
    };

    template<typename T>
    struct remove_cv_recursive<T*>
    {
      using type = typename remove_cv_recursive<T>::type*;
    };

    template<typename T>
    using remove_cv_recursive_t = typename remove_cv_recursive<T>::type;

    // Does the Type work with ostreams? This is used to implement #to_s
    template<typename T, typename = void>
    struct is_ostreamable : std::false_type {};

    template<typename T>
    struct is_ostreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T>())>> : std::true_type {};

    template<typename T>
    constexpr bool is_ostreamable_v = is_ostreamable<T>::value;

    // Is the type comparable?
    template<typename T, typename SFINAE = void>
    struct is_comparable : std::false_type {};

    template<typename T>
    struct is_comparable<T, std::void_t<
      // Does the class implement operator== and does it return a boolean value?
      decltype(std::declval<T>() == std::declval<T>() && true)
      >> : std::true_type {};

    template<typename T>
    constexpr bool is_comparable_v = is_comparable<T>::value;

    template <typename U, typename V>
    struct is_comparable<std::pair<U, V>>
    {
      static const bool value = is_comparable_v<U> && is_comparable_v<V>;
    };

    template <typename T>
    struct is_comparable<std::vector<T>>
    {
      static const bool value = is_comparable_v<T>;
    };

    template<typename>
    struct is_std_vector : std::false_type {};

    template<typename T, typename A>
    struct is_std_vector<std::vector<T, A>> : std::true_type {};

    template <typename T>
    constexpr bool is_std_vector_v = is_std_vector<T>::value;

    // -- Tuple Helpers ---
    template<typename T>
    struct tuple_shift;

    template<typename T, typename...Arg_Ts>
    struct tuple_shift<std::tuple<T, Arg_Ts...>>
    {
      using type = std::tuple<Arg_Ts...>;
    };
     
    template<template<typename, typename...> typename T, typename...Arg_Ts>
    struct tuple_map;

    template<template<typename, typename...> typename T, typename...Arg_Ts>
    struct tuple_map<T, std::tuple<Arg_Ts...>>
    {
      using type = std::tuple<T<Arg_Ts>...>;
    };


    template<typename...Arg_Ts>
    struct tuple_to_variant;

    template<typename...Arg_Ts>
    struct tuple_to_variant<std::tuple<Arg_Ts...>>
    {
      using type = std::variant<Arg_Ts...>;
    };

    template<class T>
    struct is_pointer_pointer : std::false_type {};

    template<class T>
    struct is_pointer_pointer<T**> : std::true_type {};

    template<class T>
    struct is_pointer_pointer<T** const> : std::true_type {};

    template<class T>
    struct is_pointer_pointer<T* const * const> : std::true_type {};

    template<class T>
    struct is_pointer_pointer<const T* const* const> : std::true_type {};

    template<class T>
    constexpr bool is_pointer_pointer_v = is_pointer_pointer<T>::value;

    // See https://www.cppstories.com/2022/tuple-iteration-apply/
    template <typename Tuple_T, typename Function_T>
    void for_each_tuple(Tuple_T&& tuple, Function_T&& callable)
    {
      std::apply([&callable](auto&& ...args)
        {
          (callable(std::forward<decltype(args)>(args)), ...);
        }, std::forward<Tuple_T>(tuple));
    }

    template<typename T, typename = void>
    struct is_wrapped : std::true_type {};

    template<typename T>
    struct is_wrapped<T, std::enable_if_t<std::is_fundamental_v<detail::intrinsic_type<T>> ||
                                          std::is_same_v<detail::intrinsic_type<T>, std::string>>
                     >: std::false_type {};

    template<typename T>
    constexpr bool is_wrapped_v = is_wrapped<T>::value;
  } // detail
} // Rice

#endif // Rice__detail__traits__hpp_
