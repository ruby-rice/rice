#ifndef Rice__detail__wrap_function__hpp_
#define Rice__detail__wrap_function__hpp_

// This causes problems with certain C++ libraries
#undef TYPE

#include "Exception_Handler.hpp"
#include "Wrapped_Function.hpp"
#include "../Object_defn.hpp"
#include "../Data_Object.hpp"
#include "Arguments.hpp"
#include "Auto_Function_Wrapper.hpp"
#include "Auto_Member_Function_Wrapper.hpp"

namespace Rice
{

namespace detail
{

template<typename Ret_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(),
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (*Func)();
  return new Auto_Function_Wrapper<Func, Ret_T>(func, handler, arguments);
}

template<typename Ret_T, typename ...Arg_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg_T...),
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (*Func)(Arg_T...);
  return new Auto_Function_Wrapper<Func, Ret_T, Arg_T...>(func, handler, arguments);
}

template<typename Ret_T, typename Self_T, typename ...Arg_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg_T...),
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (Self_T::*Func)(Arg_T...);
  return new Auto_Member_Function_Wrapper<Func, Ret_T, Self_T, Arg_T...>(func, handler, arguments);
}

template<typename Ret_T, typename Self_T, typename ...Arg_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg_T...) const,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (Self_T::*Func)(Arg_T...) const;
  return new Auto_Member_Function_Wrapper<Func, Ret_T, Self_T, Arg_T...>(func, handler, arguments);
}

} // namespace detail

} // namespace Rice

#endif // Rice__detail__wrap_function__hpp_

