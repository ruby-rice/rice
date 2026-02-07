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

    // Helper to detect char types
    template<typename T>
    constexpr bool is_char_type_v = std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>;

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

    // Detect if a type is complete (has a known size) or incomplete (forward-declared only)
    template<typename T, typename = void>
    struct is_complete : std::false_type {};

    template<typename T>
    struct is_complete<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};

    template<typename T>
    constexpr bool is_complete_v = is_complete<T>::value;

    template<typename T, typename = void>
    struct is_wrapped : std::true_type {};

    template<typename T>
    struct is_wrapped<T, std::enable_if_t<std::is_fundamental_v<detail::intrinsic_type<T>> ||
                                          std::is_same_v<detail::intrinsic_type<T>, std::string>>
                     >: std::false_type {};

    template<typename T>
    constexpr bool is_wrapped_v = is_wrapped<T>::value;

    // ---------- RubyKlass ------------
    template<typename, typename = std::void_t<>>
    struct has_ruby_klass : std::false_type
    {
    };

    template<typename T>
    struct has_ruby_klass<T, std::void_t<decltype(T::rubyKlass())>> : std::true_type
    {
    };

    // -- Tuple Helpers ---
    template<typename T>
    struct tuple_shift;

    template<typename T, typename...Parameter_Ts>
    struct tuple_shift<std::tuple<T, Parameter_Ts...>>
    {
      using type = std::tuple<Parameter_Ts...>;
    };

    template<typename T, typename...Parameter_Ts>
    struct tuple_unshift;

    template<typename T, typename...Parameter_Ts>
    struct tuple_unshift<T, std::tuple<Parameter_Ts...>>
    {
      using type = std::tuple<T, Parameter_Ts...>;
    };

    template<template<typename, typename...> typename T, typename...Parameter_Ts>
    struct tuple_map;

    template<template<typename, typename...> typename T, typename...Parameter_Ts>
    struct tuple_map<T, std::tuple<Parameter_Ts...>>
    {
      using type = std::tuple<T<Parameter_Ts>...>;
    };

    template<typename...Parameter_Ts>
    struct tuple_to_variant;

    template<typename...Parameter_Ts>
    struct tuple_to_variant<std::tuple<Parameter_Ts...>>
    {
      using type = std::variant<Parameter_Ts...>;
    };

    template <typename T, typename... List>
    struct is_one_of : std::disjunction<std::is_same<T, List>...> {};

    template <typename T, typename... List>
    constexpr bool is_one_of_v = is_one_of<T, List...>::value;

    template<typename... FilterTypes, typename... Arg_Ts>
    auto tuple_filter(const Arg_Ts&... args)
    {
      return std::tuple_cat([&args]()
        {
          if constexpr (is_one_of_v<Arg_Ts, FilterTypes...>)
          {
            return std::tuple<const Arg_Ts&>(args);
          }
          else
          {
            return std::tuple<>();
          }
        }()...);
    };

    // --- filter_types: recursively builds a new tuple of allowed types ---
    template <typename Tuple, typename... Allowed>
    struct tuple_filter_types;

    template <typename... Ts, typename... Allowed>
    struct tuple_filter_types<std::tuple<Ts...>, Allowed...>
    {
      using type = decltype(std::tuple_cat(std::declval<
        std::conditional_t<is_one_of_v<std::decay_t<Ts>, Allowed...>, std::tuple<Ts>, std::tuple<>>>()...));
    };

    template <typename Tuple, typename... Allowed>
    using tuple_filter_types_t = typename tuple_filter_types<Tuple, Allowed...>::type;

    template <typename T, std::size_t... Is>
    auto vector_to_tuple(const std::vector<T>& vec, std::index_sequence<Is...>)
    {
      return std::make_tuple(vec[Is]...);
    }

    template <typename Tuple, typename T, typename Seq>
    struct tuple_pad_type;

    template <typename... Ts, typename T, std::size_t... Is>
    struct tuple_pad_type<std::tuple<Ts...>, T, std::index_sequence<Is...>>
    {
      // Use Is only to repeat T N times.
      // std::conditional_t<true, T, something<Is>> is always T, but expands N times.
      using type = std::tuple<
        Ts...,
        std::conditional_t<true, T, std::integral_constant<std::size_t, Is>>...
      >;
    };

    template <typename Tuple, typename T, std::size_t N>
    using tuple_pad_type_t = typename tuple_pad_type<Tuple, T, std::make_index_sequence<N>>::type;

    // Pad tuple with values from a vector (vector size must match N)
    template <std::size_t N, typename T, typename... Ts>
    auto pad_tuple(const std::tuple<Ts...>& original, const std::vector<T>& padding)
    {
      if (padding.size() != N)
      {
        throw std::invalid_argument("Vector size doesn't match template parameter N");
      }

      auto padding_tuple = vector_to_tuple(padding, std::make_index_sequence<N>{});
      return std::tuple_cat(original, padding_tuple);
    }

    template <typename Tuple, typename... Ts>
    struct tuple_element_index_impl;

    template <typename... Ts>
    struct tuple_element_index_impl<std::tuple<>, Ts...>
    {
      static constexpr std::size_t value = 0;
    };

    template <typename First, typename... Rest, typename... Ts>
    struct tuple_element_index_impl<std::tuple<First, Rest...>, Ts...>
    {
      static constexpr bool matches = (std::is_same_v<std::decay_t<First>, Ts> || ...);
      static constexpr std::size_t value =
        matches ? 0 : 1 + tuple_element_index_impl<std::tuple<Rest...>, Ts...>::value;
    };

    template <typename Tuple, typename... Ts>
    struct tuple_element_index
    {
      static constexpr std::size_t value = tuple_element_index_impl<Tuple, Ts...>::value;
    };

    template <typename Tuple, typename... Ts>
    inline constexpr std::size_t tuple_element_index_v = tuple_element_index<Tuple, Ts...>::value;

    /* template<typename Target, typename T>
     constexpr auto tuple_predicate(T&& element)
     {
       using U = std::decay_t<T>;
       if constexpr (std::is_same_v<U, Target>)
       {
         return std::tuple<U>{ std::forward<T>(element) };
       }
       else
       {
         return std::tuple<>{};
       }
     }

     template<typename Class_T, typename... Ts>
     constexpr auto tuple_filter(Ts&&... args)
     {
       return std::tuple_cat(tuple_predicate<Class_T>(std::forward<Ts>(args))...);
     }*/



  } // detail
} // Rice

#endif // Rice__detail__traits__hpp_
