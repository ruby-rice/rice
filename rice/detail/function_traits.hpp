#ifndef Rice__detail__function_traits__hpp_
#define Rice__detail__function_traits__hpp_

namespace Rice::detail
{
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
    using nth_arg = std::tuple_element_t<N, typename functor_t::arg_types>;

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
    using nth_arg = std::tuple_element_t<N, arg_types>;

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
}

#endif // Rice__detail__function_traits__hpp_
