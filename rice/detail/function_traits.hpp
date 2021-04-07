#ifndef Rice__detail__function_traits__hpp_
#define Rice__detail__function_traits__hpp_

namespace Rice::detail
{
  // --------------   Function Traits --------------
  // Base class
  template<typename Function_T>
  struct function_traits;

  // Base definition that support functors and lambdas
  template<class Function_T>
  struct function_traits
  {
  private:
    using functor_t = function_traits<decltype(&Function_T::operator())>;

  public:
    using arg_types = typename functor_t::arg_types;

    static constexpr std::size_t arity = functor_t::arity - 1;

    template<std::size_t N>
    using nth_arg = typename std::tuple_element<N, typename functor_t::arg_types>::type;

    using return_type = typename functor_t::return_type;
    using class_type = typename std::nullptr_t;
  };

  // Specialization for functions, member functions and static member functions
  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T, Arg_Ts...)>
  {
    using arg_types = std::tuple<Arg_Ts...>;

    static constexpr std::size_t arity = sizeof...(Arg_Ts);

    template<std::size_t N>
    using nth_arg = typename std::tuple_element<N, arg_types>::type;

    using return_type = Return_T;
    using class_type = Class_T;
  };

  // Free functions and static member functions passed by pointer or reference
  template<typename Return_T, typename ...Arg_Ts>
  struct function_traits<Return_T(*)(Arg_Ts...)> : public function_traits<Return_T(std::nullptr_t, Arg_Ts...)>
  {
  };
  
  template<typename Return_T, typename ...Arg_Ts>
  struct function_traits<Return_T(&)(Arg_Ts...)> : public function_traits<Return_T(std::nullptr_t, Arg_Ts...)>
  {
  };

  // Member Functions
  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...)> : public function_traits<Return_T(Class_T, Arg_Ts...)>
  {
  };

  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...) const> : public function_traits<Return_T(Class_T, Arg_Ts...)>
  {
  };

  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...) noexcept> : public function_traits<Return_T(Class_T, Arg_Ts...)>
  {
  };

  template<typename Return_T, typename Class_T, typename...Arg_Ts>
  struct function_traits<Return_T(Class_T::*)(Arg_Ts...) const noexcept> : public function_traits<Return_T(Class_T, Arg_Ts...)>
  {
  };

  // Functors and lambdas
  template<class Function_T>
  struct function_traits<Function_T&> : public function_traits<Function_T>
  {
  };

  template<class Function_T>
  struct function_traits<Function_T&&> : public function_traits<Function_T>
  {
  };

  // --------------   Method Traits --------------
  // Declare struct
  template<typename Function_T, bool IsMethod, typename = void>
  struct method_traits;

  // Functions that do not have a self parameter
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<!IsMethod>>
  {
    using Self_T = std::nullptr_t;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  // Functions that do have a self parameter (thus we call them methods)
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<IsMethod&& std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Self_T = typename function_traits<Function_T>::nth_arg<0>;
    using Arg_Ts = typename tuple_shift<typename function_traits<Function_T>::arg_types>::type;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

  // Member functions that have an implied self parameter of an object instance
  template<typename Function_T, bool IsMethod>
  struct method_traits<Function_T, IsMethod, std::enable_if_t<IsMethod && !std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Self_T = typename function_traits<Function_T>::class_type*;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
    static constexpr std::size_t arity = std::tuple_size_v<Arg_Ts>;
  };

}

#endif // Rice__detail__function_traits__hpp_