#ifndef Rice__detail__wrap_function__hpp_
#define Rice__detail__wrap_function__hpp_

// This causes problems with certain C++ libraries
#undef TYPE

#include "Exception_Handler.hpp"
#include "Wrapped_Function.hpp"
#include "../Object_defn.hpp"
#include "../Data_Object.hpp"
#include "Arguments.hpp"

namespace Rice
{

namespace detail
{

// Used for calling Constructor
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T(*func)(Object, Arg_T...),
  std::shared_ptr<Exception_Handler> handler,
  Arguments* arguments)
{
  using Function_T = Return_T(*)(Object, Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Object, Arg_T...>(func, handler, arguments);
}

// Used by Enums
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T(*func)(Class, Arg_T...),
  std::shared_ptr<Exception_Handler> handler,
  Arguments* arguments)
{
  using Function_T = Return_T(*)(Class, Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Class, Arg_T...>(func, handler, arguments);
}

// A plain function or static member call
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T (*func)(Arg_T...),
    std::shared_ptr<Exception_Handler> handler,
    Arguments* arguments)
{
  using Function_T = Return_T(*)(Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, std::nullptr_t, Arg_T...>(func, handler, arguments);
}

// Call a member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Return_T (Self_T::*func)(Arg_T...),
    std::shared_ptr<Exception_Handler> handler,
    Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Self_T*, Arg_T...>(func, handler, arguments);
}

// Call a const member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Return_T (Self_T::*func)(Arg_T...) const,
    std::shared_ptr<Exception_Handler> handler,
    Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...) const;
  return new Wrapped_Function<Function_T, Return_T, Self_T, Arg_T...>(func, handler, arguments);
}

} // namespace detail

} // namespace Rice

#endif // Rice__detail__wrap_function__hpp_

