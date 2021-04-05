#ifndef Rice__detail__Native_Function__hpp_
#define Rice__detail__Native_Function__hpp_

#include "ruby.hpp"
#include "NativeArg.hpp"
#include "Exception_Handler_defn.hpp"
#include "Arguments.hpp"

namespace Rice::detail
{
  /* This class wraps a native function call. The most important template parameters
     are Self_T and Return_T.

     Self_T can have one of the following values:

     * std::null_ptr - There is no reciever, thus this is a function or static member function
     * Object - This is used for calling constructors.
     * Class - This is used for translating enums values
     * C++ Object - This is the most common value and is used to invoke a member function
       on a wrapped instance of a C++ object

     Return_T which specifies the return value. It can either be:

     * void - Meaning there is no return value. This is mapped to Qnil in Ruby
     * any other possible value */

  template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
  class Native_Function
  {
  public:
    using Native_Return_T = Return_T;

    // NativeTypes are the types that we pass to the native function. They may or may not include
    // the receiver as the first argument. This alias makes it possible to have just one
    // implemenation of invokeNative
    using Native_Arg_Ts = typename std::conditional_t<std::is_same_v<Self_T, std::nullptr_t>,
      std::tuple<Arg_Ts...>,
      std::tuple<Self_T, Arg_Ts...>>;

    // Static member function that Ruby calls
    static VALUE call(int argc, VALUE* argv, VALUE self);

  public:
    Native_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

    // Invokes the wrapped function
    VALUE operator()(int argc, VALUE* argv, VALUE self);

  private:
    // Create NativeArgs which are used to convert values from Ruby to C++
    template<std::size_t... I>
    std::tuple<NativeArg<Arg_Ts>...> createNativeArgs(std::index_sequence<I...>& indices);

    // Convert Ruby argv pointer to Ruby values
    std::vector<VALUE> getRubyValues(int argc, VALUE* argv);

    // Convert Ruby values to C++ values
    template<typename std::size_t... I>
    std::tuple<Arg_Ts...> getNativeValues(std::vector<VALUE>& values, std::tuple<NativeArg<Arg_Ts>...>& nativeArgs, std::index_sequence<I...>& indices);

    // Figure out what self is
    Self_T getSelf(VALUE self);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, std::vector<VALUE>& rubyValues);

    // Call the underlying C++ function
    VALUE invokeNativeFunction(std::tuple<Arg_Ts...>& nativeArgs);
    VALUE invokeNativeMethod(VALUE self, std::tuple<Arg_Ts...>& nativeArgs);

  private:
    Function_T func_;
    std::shared_ptr<Exception_Handler> handler_;
    std::unique_ptr<Arguments> arguments_;
  };

  // A plain function or static member call
  template<typename Return_T, typename ...Arg_T>
  auto* Make_Native_Function(Return_T(*func)(Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Lambda function that does not take Self as first parameter
  template<typename Func_T>
  auto* Make_Native_Function(Func_T&& func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // A plain function or static member call
  template<typename Return_T, typename Self_T, typename ...Arg_T>
  auto* Make_Native_Function_With_Self(Return_T(*func)(Self_T, Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Lambda function with Self_T as first argument
  template<typename Func_T>
  auto* Make_Native_Function_With_Self(Func_T&& func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Call a member function on a C++ object
  template<typename Return_T, typename Self_T, typename ...Arg_T>
  auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Call a noexcept member function on a C++ object
  template<typename Return_T, typename Self_T, typename ...Arg_T>
  auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...) noexcept, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Call a const member function on a C++ object
  template<typename Return_T, typename Self_T, typename ...Arg_T>
  auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Call a const noexcept member function on a C++ object
  template<typename Return_T, typename Self_T, typename ...Arg_T>
  auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...) const noexcept, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);
}
#include "Native_Function.ipp"

#endif // Rice__detail__Native_Function__hpp_
\