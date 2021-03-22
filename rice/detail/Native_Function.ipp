#include <array>
#include <algorithm>

#include "method_data.hpp"
#include "to_ruby_defn.hpp"
#include "../ruby_try_catch.hpp"

namespace Rice
{

namespace detail
{

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
VALUE Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
call(int argc, VALUE* argv, VALUE self)
{
  using Wrapper_T = Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>;
  Wrapper_T* wrapper = detail::MethodData::data<Wrapper_T*>();
  return wrapper->operator()(argc, argv, self);
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
Native_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
    : func_(func), handler_(handler), arguments_(arguments)
{
  if (!arguments_)
  {
    arguments_ = std::make_unique<Arguments>();
  }

  // Ruby takes ownership of types returned by value. We do this here so that users
  // don't have to be bothered to specify this in define_method. Note we *must* set 
  // this correctly because To_Ruby<T>::convert routines work with const T& and thus
  // if ownership is false they will store a reference to the value and that will
  // result in a crash.
  if (!std::is_reference_v<Return_T> && !std::is_pointer_v<Return_T>)
  {
    arguments_->takeOwnership();
  }
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
std::vector<VALUE> Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
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

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
template<std::size_t... I>
std::tuple<Arg_Ts...> Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
getNativeValues(std::vector<VALUE>& values, std::tuple<NativeArg<Arg_Ts>...>& nativeArgs, std::index_sequence<I...>& indices)
{
  // Convert each Ruby value to its native value. Check each Ruby nil value to see if it has
  // a default argument, and if yes, use that. Otherwise use NativeArg<Arg_Ts> to convert
  // the Ruby value to a native value. Note that for fundamental types NativeArg<Arg_Ts> 
  // will keep a copy of the native value so it can be passed by reference or pointer to a
  // native function.
  return std::forward_as_tuple((values[I] == Qnil && this->arguments_->isOptional(I) ?
                                this->arguments_->template defaultValue<Arg_Ts>(I) :
                                std::get<I>(nativeArgs).nativeValue(values[I]))...);
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
Self_T Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
getSelf(VALUE self)
{
  // This is used for functions and static members
  if constexpr (std::is_same_v<Self_T, void>)
  {
    return nullptr;
  }
  // This is used for passing VALUE klass or VALUE self
  else if constexpr (std::is_same_v<Self_T, VALUE>)
  {
    return self;
  }
  else
  {
    // This is used for member functions - we are returning a pointer to an object
    return From_Ruby<Self_T>::convert(self);
  }
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
VALUE Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
invokeNativeFunction(std::tuple<Arg_Ts...>& nativeArgs)
{
  if constexpr (std::is_void_v<Return_T>)
  {
    std::apply(this->func_, nativeArgs);
    return Qnil;
  }
  else
  {
    // Call the native method and get the result
    Return_T nativeResult = std::apply(this->func_, nativeArgs);
    return To_Ruby<Return_T>::convert(nativeResult, this->arguments_->isOwner());
  }
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
VALUE Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
invokeNativeMethod(VALUE self, std::tuple<Arg_Ts...>& nativeArgs)
{
  Self_T receiver = this->getSelf(self);
  std::tuple<Self_T, Arg_Ts...> selfAndNativeArgs = std::tuple_cat(std::forward_as_tuple(receiver), nativeArgs);

  if constexpr (std::is_void_v<Return_T>)
  {
    std::apply(this->func_, selfAndNativeArgs);
    return Qnil;
  }
  else
  {
    // Call the native method
    Return_T nativeResult = std::apply(this->func_, selfAndNativeArgs);

    // Special handling if the method returns self. If so we do not want
    // to create a new Ruby wrapper object and instead return self.
    if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Self_T>>)
    {
      if constexpr (std::is_pointer_v<Return_T> && std::is_pointer_v<Self_T>)
      {
        if (nativeResult == receiver)
          return self;
      }
      else if constexpr (std::is_pointer_v<Return_T> && std::is_reference_v<Self_T>)
      {
        if (nativeResult == &receiver)
          return self;
      }
      else if constexpr (std::is_reference_v<Return_T> && std::is_pointer_v<Self_T>)
      {
        if (&nativeResult == receiver)
          return self;
      }
      else if constexpr (std::is_reference_v<Return_T> && std::is_reference_v<Self_T>)
      {
        if (&nativeResult == &receiver)
          return self;
      }
    }

    return To_Ruby<Return_T>::convert(nativeResult, this->arguments_->isOwner());
  }
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
void Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
checkKeepAlive(VALUE self, std::vector<VALUE>& rubyValues)
{
  Wrapper* wrapper = getWrapper(self);

  for (const Arg& arg : (*this->arguments_))
  {
    if (arg.isKeepAlive)
    {
      wrapper->addKeepAlive(rubyValues[arg.position]);
    }
  }
}

template<typename Function_T, typename Return_T, typename Self_T, typename... Arg_Ts>
VALUE Native_Function<Function_T, Return_T, Self_T, Arg_Ts...>::
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

    // Check if any rubyValues need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, rubyValues);

    // Now call the native method
    if constexpr (std::is_same_v<Self_T, std::nullptr_t>)
    {
      return this->invokeNativeFunction(nativeValues);
    }
    else
    {
      return this->invokeNativeMethod(self, nativeValues);
    }
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

// Lambda helpers
template<typename Function_T, typename Return_T, typename Class_T, typename ...Arg_T>
auto* lambda_helper(Function_T&& func, Return_T(Class_T::*)(Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  return new Native_Function<Function_T, Return_T, std::nullptr_t, Arg_T...>(std::forward<Function_T>(func), handler, arguments);
}

template<typename Function_T, typename Return_T, typename Class_T, typename Self_T, typename ...Arg_T>
auto* lambda_helper_with_self(Function_T&& func, Return_T(Class_T::*)(Self_T, Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  return new Native_Function<Function_T, Return_T, Self_T, Arg_T...>(std::forward<Function_T>(func), handler, arguments);
}

// A plain function or static member call
template<typename Return_T, typename ...Arg_T>
auto* Make_Native_Function(Return_T(*func)(Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(*)(Arg_T...);
  return new Native_Function<Function_T, Return_T, std::nullptr_t, Arg_T...>(func, handler, arguments);
}

// Lambda function that does not take Self as first parameter
template<typename Func_T>
auto* Make_Native_Function(Func_T&& func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = decltype(&Func_T::operator());
  return lambda_helper(std::forward<Func_T>(func), (Function_T)nullptr, handler, arguments);
}

// A plain function or static member call
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* Make_Native_Function_With_Self(Return_T(*func)(Self_T, Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(*)(Self_T, Arg_T...);
  return new Native_Function<Function_T, Return_T, Self_T, Arg_T...>(func, handler, arguments);
}

// Lambda function with Self_T as first argument
template<typename Func_T>
auto* Make_Native_Function_With_Self(Func_T&& func, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = decltype(&Func_T::operator());
  return lambda_helper_with_self(std::forward<Func_T>(func), (Function_T)nullptr, handler, arguments);
}

// Call a member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...), std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...);
  return new Native_Function<Function_T, Return_T, Self_T*, Arg_T...>(func, handler, arguments);
}

// Call a const member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...) const, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...) const;
  return new Native_Function<Function_T, Return_T, Self_T*, Arg_T...>(func, handler, arguments);
}

// Call a const member function on a C++ object
template<typename Return_T, typename Self_T, typename ...Arg_T>
auto* Make_Native_Function_With_Self(Return_T(Self_T::* func)(Arg_T...) const noexcept, std::shared_ptr<Exception_Handler> handler, Arguments* arguments)
{
  using Function_T = Return_T(Self_T::*)(Arg_T...) const noexcept;
  return new Native_Function<Function_T, Return_T, Self_T*, Arg_T...>(func, handler, arguments);
}

} // detail
} // Rice
