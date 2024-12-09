#ifndef Rice__detail__method_traits__hpp_
#define Rice__detail__method_traits__hpp_

#include <tuple>

namespace Rice::detail
{
  // Declare struct
  template<typename Function_T, bool IsMethod, typename = void>
  struct method_traits;

  // Functions that do not have a self parameter:
  //   doSomething(int a)
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<!IsMethod>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = std::nullptr_t;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  /* Functions that have a self parameter and thus we treat them as free standing
    "methods" versus member functions. 
    
       doSomething(VALUE self, int a) */
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<IsMethod && std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = typename function_traits<Function_T>::template nth_arg<0>;
    using Arg_Ts = typename tuple_shift<typename function_traits<Function_T>::arg_types>::type;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  // Member functions that have an implied self parameter of an object instance
  //   foo.doSomething(int a)
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<IsMethod && !std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Class_T = typename function_traits<Function_T>::class_type;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };
}
#endif // Rice__detail__method_traits__hpp_
