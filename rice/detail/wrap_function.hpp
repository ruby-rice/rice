#ifndef Rice__detail__wrap_function__hpp_
#define Rice__detail__wrap_function__hpp_

#include <functional>

#include "Exception_Handler.hpp"
#include "Wrapped_Function.hpp"
#include "Arguments.hpp"

namespace Rice
{

// TODO - forward declarations
class Class;
class Object;

namespace detail
{

// Used for calling Constructor
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T(*func)(Object, Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(*)(Object, Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Object, Arg_T...>(func, handler, arguments);
}

// Used by Enums
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T(*func)(Class, Arg_T...), std::shared_ptr<Exception_Handler> handler,  Arguments* arguments)
{
  using Function_T = Return_T(*)(Class, Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Class, Arg_T...>(func, handler, arguments);
}

// A plain function or static member call
template<typename Return_T, typename ...Arg_T>
auto* wrap_function(Return_T (*func)(Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(*)(Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, std::nullptr_t, Arg_T...>(func, handler, arguments);
}

// Lambda function helper
template<typename Function_T, typename Return_T, typename Class_T, typename ...Arg_T>
auto* wrap_function(Function_T&& func, Return_T(Class_T::*)(Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  return new Wrapped_Function<Function_T, Return_T, std::nullptr_t, Arg_T...>(std::forward<Function_T>(func), handler, arguments);
}

/*template<typename Function_T, typename Return_T, typename Class_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Function_T&& func, Return_T(Class_T::*)(Self_T, Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  return new Wrapped_Function<Function_T, Return_T, Self_T, Arg_T...>(std::forward<Function_T>(func), handler, arguments);
}*/

// Lambda function
template<typename Func_T>
auto* wrap_function(Func_T&& func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = decltype(&Func_T::operator());
  return wrap_function(std::forward<Func_T>(func), (Function_T)nullptr, handler, arguments);
}

// Call a member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Return_T (Self_T::*func)(Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...);
  return new Wrapped_Function<Function_T, Return_T, Self_T*, Arg_T...>(func, handler, arguments);
}

// Call a const member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* wrap_function(Return_T (Self_T::*func)(Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...) const;
  return new Wrapped_Function<Function_T, Return_T, Self_T, Arg_T...>(func, handler, arguments);
}

} // namespace detail

} // namespace Rice

#endif // Rice__detail__wrap_function__hpp_