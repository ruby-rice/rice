#ifndef Rice__detail__Native_Function__hpp_
#define Rice__detail__Native_Function__hpp_

#include "ruby.hpp"
#include "NativeArg.hpp"
#include "Exception_Handler_defn.hpp"
#include "MethodInfo.hpp"
#include "function_traits.hpp"

namespace Rice::detail
{
  // Declare struct
  template<typename Function_T, bool IsMethod, typename = void>
  struct self_traits;

  // Functions that do not have a self parameter
  template<typename Function_T, bool IsMethod>
  struct self_traits<Function_T, IsMethod, std::enable_if_t<!IsMethod>>
  {
    using Self_T = std::nullptr_t;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
  };

  // Functions that do have a self parameter (thus we call them methods)
  template<typename Function_T, bool IsMethod>
  struct self_traits<Function_T, IsMethod, std::enable_if_t<IsMethod&& std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Self_T = typename function_traits<Function_T>::nth_arg<0>;
    using Arg_Ts = typename tuple_shift<typename function_traits<Function_T>::arg_types>::type;
  };

  // Member functions that have an implied self parameter of an object instance
  template<typename Function_T, bool IsMethod>
  struct self_traits<Function_T, IsMethod, std::enable_if_t<IsMethod && !std::is_same_v<typename function_traits<Function_T>::class_type, std::nullptr_t>>>
  {
    using Self_T = typename function_traits<Function_T>::class_type*;
    using Arg_Ts = typename function_traits<Function_T>::arg_types;
  };

  template<typename Function_T, bool IsMethod>
  class Native_Function
  {
  public:
    using Return_T = typename function_traits<Function_T>::return_type;
    using Self_T = typename self_traits<Function_T, IsMethod>::Self_T;
    using Arg_Ts = typename self_traits<Function_T, IsMethod>::Arg_Ts;
    using Native_Arg_Ts = typename tuple_map<NativeArg, Arg_Ts>::type;

    // Static member function that Ruby calls
    static VALUE call(int argc, VALUE* argv, VALUE self);

  public:
    Native_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, MethodInfo* methodInfo);

    // Invokes the wrapped function
    VALUE operator()(int argc, VALUE* argv, VALUE self);

  private:
    // Create NativeArgs which are used to convert values from Ruby to C++
    template<std::size_t... I>
    Native_Arg_Ts createNativeArgs(std::index_sequence<I...>& indices);

    // Convert Ruby argv pointer to Ruby values
    std::vector<VALUE> getRubyValues(int argc, VALUE* argv);

    // Convert Ruby values to C++ values
    template<typename std::size_t... I>
    Arg_Ts getNativeValues(std::vector<VALUE>& values, Native_Arg_Ts& nativeArgs, std::index_sequence<I...>& indices);

    // Figure out what self is
    Self_T getSelf(VALUE self);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, std::vector<VALUE>& rubyValues);

    // Call the underlying C++ function
    VALUE invokeNativeFunction(Arg_Ts& nativeArgs);
    VALUE invokeNativeMethod(VALUE self, Arg_Ts& nativeArgs);

  private:
    Function_T func_;
    std::shared_ptr<Exception_Handler> handler_;
    std::unique_ptr<MethodInfo> methodInfo_;
  };
}
#include "Native_Function.ipp"

#endif // Rice__detail__Native_Function__hpp_