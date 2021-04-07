#include <array>
#include <algorithm>

#include "method_data.hpp"
#include "to_ruby_defn.hpp"
#include "NativeReturn.hpp"
#include "../ruby_try_catch.hpp"

namespace Rice::detail
{
  template<typename Function_T, bool IsMethod>
  VALUE Native_Function<Function_T, IsMethod>::call(int argc, VALUE* argv, VALUE self)
  {
    using Wrapper_T = Native_Function<Function_T, IsMethod>;
    Wrapper_T* wrapper = detail::MethodData::data<Wrapper_T*>();
    return wrapper->operator()(argc, argv, self);
  }

  template<typename Function_T, bool IsMethod>
  Native_Function<Function_T, IsMethod>::Native_Function(Function_T func, std::shared_ptr<Exception_Handler> handler, MethodInfo* methodInfo)
    : func_(func), handler_(handler), methodInfo_(methodInfo)
  {
    // Ruby takes ownership of types returned by value. We do this here so that users
    // don't have to be bothered to specify this in define_method. Note we *must* set 
    // this correctly because To_Ruby<T>::convert routines work with const T& and thus
    // if ownership is false they will store a reference to the value and that will
    // result in a crash.
    if (!std::is_reference_v<Return_T> && !std::is_pointer_v<Return_T>)
    {
      methodInfo_->takeOwnership();
    }
  }

  template<typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename Native_Function<Function_T, IsMethod>::Native_Arg_Ts Native_Function<Function_T, IsMethod>::createNativeArgs(std::index_sequence<I...>& indices)
  {
    return std::make_tuple(NativeArg<std::tuple_element<I, Arg_Ts>::type>(this->methodInfo_->arg(I))...);
  }

  template<typename Function_T, bool IsMethod>
  std::vector<VALUE> Native_Function<Function_T, IsMethod>::getRubyValues(int argc, VALUE* argv)
  {
    // Setup a tuple to contain required methodInfo to rb_scan_args
    std::string scanFormat = this->methodInfo_->formatString();
    std::tuple<int, VALUE*, const char*> rbScanMandatory = std::forward_as_tuple(argc, argv, scanFormat.c_str());

    // Create a vector to store the variable number of Ruby Values
    std::vector<VALUE> rbScanArgsOptional(std::tuple_size_v<Arg_Ts>, Qnil);

    // Convert the vector to an array so it can then be concatenated to a tuple
    std::array<VALUE*, std::tuple_size_v<Arg_Ts>> rbScanArgsOptionalPointers;
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

  template<typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename Native_Function<Function_T, IsMethod>::Arg_Ts Native_Function<Function_T, IsMethod>::getNativeValues(std::vector<VALUE>& values,
    typename Native_Function<Function_T, IsMethod>::Native_Arg_Ts& nativeArgs, std::index_sequence<I...>& indices)
  {
    // Convert each Ruby value to its native value. Check each Ruby nil value to see if it has
    // a default argument, and if yes, use that. Otherwise use NativeArg<Arg_Ts> to convert
    // the Ruby value to a native value. Note that for fundamental types NativeArg<Arg_Ts> 
    // will keep a copy of the native value so it can be passed by reference or pointer to a
    // native function.
    return std::forward_as_tuple(std::get<I>(nativeArgs).nativeValue(values[I])...);
  }

  template<typename Function_T, bool IsMethod>
  typename Native_Function<Function_T, IsMethod>::Self_T Native_Function<Function_T, IsMethod>::getSelf(VALUE self)
  {
    // There is no self parameter
    if constexpr (std::is_same_v<Self_T, std::nullptr_t>)
    {
      return nullptr;
    }
    // Self parameter is a Ruby VALUE so no conversion is needed
    else if constexpr (std::is_same_v<Self_T, VALUE>)
    {
      return self;
    }
    // Self parameter is an object and thus needs to be unwrapped from Ruby
    else
    {
      return From_Ruby<Self_T>::convert(self);
    }
  }

  template<typename Function_T, bool IsMethod>
  VALUE Native_Function<Function_T, IsMethod>::invokeNativeFunction(Arg_Ts& nativeArgs)
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
      
      // Create a wrapper object to convert to Ruby
      NativeReturn<Return_T> nativeReturn(this->methodInfo_->returnInfo);

      // Return the result
      return nativeReturn.getValue(nativeResult);
    }
  }

  template<typename Function_T, bool IsMethod>
  VALUE Native_Function<Function_T, IsMethod>::invokeNativeMethod(VALUE self, Arg_Ts& nativeArgs)
  {
    Self_T receiver = this->getSelf(self);
    auto selfAndNativeArgs = std::tuple_cat(std::forward_as_tuple(receiver), nativeArgs);

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

      return To_Ruby<Return_T>::convert(nativeResult, this->methodInfo_->isOwner());
    }
  }

  template<typename Function_T, bool IsMethod>
  void Native_Function<Function_T, IsMethod>::checkKeepAlive(VALUE self, std::vector<VALUE>& rubyValues)
  {
    Wrapper* wrapper = getWrapper(self);

    for (const Arg& arg : (*this->methodInfo_))
    {
      if (arg.isKeepAlive)
      {
        wrapper->addKeepAlive(rubyValues[arg.position]);
      }
    }
  }

  template<typename Function_T, bool IsMethod>
  VALUE Native_Function<Function_T, IsMethod>::operator()(int argc, VALUE* argv, VALUE self)
  {
    try
    {
      // Get the ruby values
      std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv);

      auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

      // Create a tuple of NativeArgs that will convert the Ruby values to native values. For 
      // builtin types NativeArgs will keep a copy of the native value so that it 
      // can be passed by reference or pointer to the native function. For non-builtin types
      // it will just pass the value through.
      Native_Arg_Ts nativeArgs = this->createNativeArgs(indices);

      // Convert the Ruby values to native values
      Arg_Ts nativeValues = this->getNativeValues(rubyValues, nativeArgs, indices);

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
}
