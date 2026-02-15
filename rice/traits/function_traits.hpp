#ifndef Rice__detail__function_traits__hpp_
#define Rice__detail__function_traits__hpp_

#include <tuple>

namespace Rice::detail
{
  // Base class
  template<typename Function_T>
  struct function_traits;

  template<typename Return_T, typename Class_T, typename...Parameter_Ts>
  struct function_traits<Return_T(Class_T, Parameter_Ts...)>
  {
    using arg_types = std::tuple<Parameter_Ts...>;

    static constexpr std::size_t arity = sizeof...(Parameter_Ts);

    template<std::size_t N>
    using nth_arg = typename std::tuple_element_t<N, arg_types>;

    using return_type = Return_T;
    using class_type = Class_T;
  };

  // Functors and lambdas with operator()
  template<typename Function_T>
  struct function_traits : public function_traits<decltype(&Function_T::operator())>
  {
  private:
    using functor_t = function_traits<decltype(&Function_T::operator())>;

  public:
    using arg_types = typename functor_t::arg_types;
    static constexpr std::size_t arity = functor_t::arity;
    using class_type = std::nullptr_t;
  };

  // Lvalue references to functors and lambdas - strip the reference and defer
  // to the base functor specialization. This allows named lambda variables (lvalues)
  // to be passed to define_method in addition to inline temporaries (rvalues).
  template<typename Function_T>
  struct function_traits<Function_T&> : public function_traits<Function_T>
  {
  };

  // C functions and static member functions passed by pointer
  template<typename Return_T, typename ...Parameter_Ts>
  struct function_traits<Return_T(*)(Parameter_Ts...)> : public function_traits<Return_T(std::nullptr_t, Parameter_Ts...)>
  {
    using Function_T = Return_T(*)(Parameter_Ts...);
  };

  // noexcept C functions and static member functions passed by pointer
  template<typename Return_T, typename ...Parameter_Ts>
  struct function_traits<Return_T(*)(Parameter_Ts...) noexcept> : public function_traits<Return_T(std::nullptr_t, Parameter_Ts...)>
  {
    using Function_T = Return_T(*)(Parameter_Ts...) noexcept;
  };

  // C functions passed by pointer that take one or more defined parameter than a variable
  // number of parameters (the second ...)
  template<typename Return_T, typename ...Parameter_Ts>
  struct function_traits<Return_T(*)(Parameter_Ts..., ...)> : public function_traits<Return_T(std::nullptr_t, Parameter_Ts...)>
  {
  };

  // C Functions or static member functions passed by reference
  template<typename Return_T, typename ...Parameter_Ts>
  struct function_traits<Return_T(&)(Parameter_Ts...)> : public function_traits<Return_T(std::nullptr_t, Parameter_Ts...)>
  {
  };

  // noexcept C functions or static member functions passed by reference
  template<typename Return_T, typename ...Parameter_Ts>
  struct function_traits<Return_T(&)(Parameter_Ts...) noexcept> : public function_traits<Return_T(std::nullptr_t, Parameter_Ts...)>
  {
  };

  // Member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Parameter_Ts>
  struct function_traits<Return_T(Class_T::*)(Parameter_Ts...)> : public function_traits<Return_T(Class_T*, Parameter_Ts...)>
  {
  };

  // const member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Parameter_Ts>
  struct function_traits<Return_T(Class_T::*)(Parameter_Ts...) const> : public function_traits<Return_T(Class_T*, Parameter_Ts...)>
  {
  };

  // noexcept member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Parameter_Ts>
  struct function_traits<Return_T(Class_T::*)(Parameter_Ts...) noexcept> : public function_traits<Return_T(Class_T*, Parameter_Ts...)>
  {
  };


  // const noexcept member Functions on C++ classes
  template<typename Return_T, typename Class_T, typename...Parameter_Ts>
  struct function_traits<Return_T(Class_T::*)(Parameter_Ts...) const noexcept> : public function_traits<Return_T(Class_T*, Parameter_Ts...)>
  {
  };

  /*// Functors and lambdas
  template<class Function_T>
  struct function_traits<Function_T&> : public function_traits<Function_T>
  {
  };

  template<class Function_T>
  struct function_traits<Function_T&&> : public function_traits<Function_T>
  {
  };*/
}
#endif // Rice__detail__function_traits__hpp_
