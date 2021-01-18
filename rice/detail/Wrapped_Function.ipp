#ifndef Rice__detail__Wrapped_Function__ipp_
#define Rice__detail__Wrapped_Function__ipp_

#include <array>
#include <algorithm>

#include "method_data.hpp"
#include "../ruby_try_catch.hpp"
#include "to_ruby_defn.hpp"
#include "../Class_defn.hpp"
#include "../Data_Object_defn.hpp"

namespace Rice
{

namespace detail
{

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
VALUE Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
call(int argc, VALUE* argv, VALUE self)
{
  using WrapperType = Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>;

  Data_Object<WrapperType> data(detail::method_data());
  WrapperType* wrapper = (WrapperType*)data.get();
  return wrapper->operator()(argc, argv, self);
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
Wrapped_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
    : func_(func), handler_(handler)
  {
    if (arguments == 0)
    {
      arguments_ = new Arguments();
    }
    else
    {
      arguments_ = arguments;
    }
  }

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
~Wrapped_Function()
{
  delete arguments_;
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
std::vector<VALUE> Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
getRubyValues(int argc, VALUE* argv)
{
  // Setup a tuple to contain required arguments to rb_scan_args
  std::string scanFormat = this->arguments_->formatString(sizeof...(Arg_Ts));
  std::tuple<int, VALUE*, const char*> rbScanMandatory = std::forward_as_tuple(argc, argv, scanFormat.c_str());

  // Create a vector to store the variable number of Ruby Values
  std::vector<VALUE> rbScanArgsOptional(sizeof...(Arg_Ts), Qnil);

  // Convert the vector to an array so it can then be concatenated to a tuple
  std::array<VALUE*, sizeof...(Arg_Ts)> rbScanArgsOptionalPointers;
  std::transform(rbScanArgsOptional.begin(), rbScanArgsOptional.end(), rbScanArgsOptionalPointers.begin(),
    [](VALUE& value)
    {
      return &value;
    });

  // Combine the tuples and call rb_scan_args
  auto rbScanArgs = std::tuple_cat(rbScanMandatory, rbScanArgsOptionalPointers);
  std::apply(rb_scan_args, rbScanArgs);

  return rbScanArgsOptional;
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
template<std::size_t... I>
std::tuple<Arg_Ts...> Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
getNativeValues(std::vector<VALUE>& values, std::tuple<NativeArg<Arg_Ts>...>& nativeArgs, std::index_sequence<I...>& indices)
{
  // Convert each Ruby value to its native value. Check each Ruby nil value to see if it has
  // a default argument, and if yes, use that. Otherwise use NativeArg<Arg_Ts> to convert
  // the Ruby value to a native value. Note that for fundamental types NativeArg<Arg_Ts> 
  // will keep a copy of the native value so it can be passed by reference or pointer to a
  // native function.
  return std::forward_as_tuple((values[I] == Qnil && this->arguments_->isOptional(I) ?
                                this->arguments_->defaultValue<Arg_Ts>(I) :
                                std::get<I>(nativeArgs).nativeValue(values[I]))...);
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
Receiver_T Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
getReceiver(VALUE receiver)
{
  // Return the receiver.
  if constexpr (std::is_same<Receiver_T, Object>::value)
  {
    // This is used for Rice::Constructor
    return Object(receiver);
  }
  else if constexpr (std::is_same<Receiver_T, Class>::value)
  {
    // This is used for Rice::Enum
    return Class(receiver);
  }
  else if constexpr (std::is_same<Receiver_T, void>::value)
  {
    // This is used for functions and static members
    return nullptr;
  }
  else
  {
    // This is used for member functions - we are returning a pointer to an object
    return From_Ruby<Receiver_T>::convert(receiver);
  }
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
VALUE Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
invokeNative(NativeTypes& nativeArgs)
{
  // Call the native function
  if constexpr (std::is_void_v<Return_T>)
  {
    std::apply(this->func_, nativeArgs);
    return Qnil;
  }
  else
  {
    Return_T result = std::apply(this->func_, nativeArgs);
    return To_Ruby<Return_T>::convert(result);
  }
}

template<typename Function_T, typename Return_T, typename Receiver_T, typename... Arg_Ts>
VALUE Wrapped_Function<Function_T, Return_T, Receiver_T, Arg_Ts...>::
operator()(int argc, VALUE* argv, VALUE self)
{
  try
  {
    // Get the ruby values
    std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv);

    // Create a tuple of NativeArgs that will convert the Ruby values to native values. For 
    // fundamental types NativeArgs will also keep a copy of the native value so that it 
    // can be passed by reference or pointer to the native function.
    std::tuple<NativeArg<Arg_Ts>...> nativeArgs;

    // Convert the Ruby values to native values
    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};
    std::tuple<Arg_Ts...> nativeValues = this->getNativeValues(rubyValues, nativeArgs, indices);

    // Now call the native method
    if constexpr (std::is_same_v<Receiver_T, std::nullptr_t>)
    {
      return this->invokeNative(nativeValues);
    }
    else
    {
      Receiver_T receiver = this->getReceiver(self);
      std::tuple<Receiver_T, Arg_Ts...> nativeArgs = std::tuple_cat(std::tuple(receiver), nativeValues);
      return this->invokeNative(nativeArgs);
    }
    return Qnil;
  }
  catch (...)
  {
    RUBY_TRY
    {
      return this->handler_->handle_exception();
    }
    RUBY_CATCH
  }
}

} // detail

} // Rice

#endif // Rice__detail__Wrapped_Function__ipp_
