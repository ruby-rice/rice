#ifndef Rice__detail__Wrapped_Function__hpp_
#define Rice__detail__Wrapped_Function__hpp_

#include "ruby.hpp"
#include "NativeArg.hpp"

namespace Rice
{

namespace detail
{
/* This class wraps a native function call. The most important template parameters
   are Receiver_T and Return_T.

   Receiver_T can have one of the following values:
   
   * std::null_ptr - There is no reciever, thus this is a function or static member function
   * Object - This is used for calling constructors.
   * Class - This is used for translating enums values
   * C++ Object - This is the most common value and is used to invoke a member function 
     on a wrapped instance of a C++ object
   
   Return_T which specifies the return value. It can either be:

   * void - Meaning there is no return value. This is mapped to Qnil in Ruby
   * any other possible value */
  
template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
class Wrapped_Function
{
// NativeTypes are the types that we pass to the native function. They may or may not include
// the receiver as the first argument. This alias makes it possible to have just one
// implemenation of invokeNative
using NativeTypes = typename std::conditional_t<std::is_same_v<Receiver_T, std::nullptr_t>,
                                                std::tuple<Arg_Ts...>, 
                                                std::tuple<Receiver_T, Arg_Ts...>>;

public:
  // Static member function that Ruby calls
  static VALUE call(int argc, VALUE* argv, VALUE self);

public:
  Wrapped_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments);

  // Invokes the wrapped function
  VALUE operator()(int argc, VALUE* argv, VALUE self);

private:
  // Convert Ruby argv pointer to Ruby values
  std::vector<VALUE> getRubyValues(int argc, VALUE* argv);

  // Convert Ruby values to C++ values
  template<typename std::size_t... I>
  std::tuple<Arg_Ts...> getNativeValues(std::vector<VALUE>& values, std::tuple<NativeArg<Arg_Ts>...>& nativeArgs, std::index_sequence<I...>& indices);

  // Figure out the receiver of the function call
  Receiver_T getReceiver(VALUE receiver);

  // Do we need to keep alive any arguments?
  void checkKeepAlive(VALUE self, std::vector<VALUE>& rubyValues);

  // Call the underlying C++ function
  VALUE invokeNative(NativeTypes& nativeArgs);

private:
  Function_T func_;
  std::shared_ptr<Exception_Handler> handler_;
  std::unique_ptr<Arguments> arguments_;
};

} // detail

} // Rice


#endif // Rice__detail__Wrapped_Function__hpp_
