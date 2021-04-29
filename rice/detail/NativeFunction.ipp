#include <array>
#include <algorithm>
#include <stdexcept>

#include "method_data.hpp"
#include "to_ruby_defn.hpp"
#include "../ruby_try_catch.hpp"

namespace Rice::detail
{
  template<typename Function_T, bool IsMethod>
  VALUE NativeFunction<Function_T, IsMethod>::call(int argc, VALUE* argv, VALUE self)
  {
    using Wrapper_T = NativeFunction<Function_T, IsMethod>;
    Wrapper_T* wrapper = detail::MethodData::data<Wrapper_T*>();
    return wrapper->operator()(argc, argv, self);
  }

  template<typename Function_T, bool IsMethod>
  NativeFunction<Function_T, IsMethod>::NativeFunction(Function_T func, std::shared_ptr<Exception_Handler> handler, MethodInfo* methodInfo)
    : func_(func), handler_(handler), methodInfo_(methodInfo)
  {
    // Create a tuple of NativeArgs that will convert the Ruby values to native values. For 
    // builtin types NativeArgs will keep a copy of the native value so that it 
    // can be passed by reference or pointer to the native function. For non-builtin types
    // it will just pass the value through.
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    this->fromRubys_ = this->createFromRuby(indices);

    this->toRuby_ = this->createToRuby();
  }

  template<typename Function_T, bool IsMethod>
  template<typename T, std::size_t I>
  From_Ruby<T> NativeFunction<Function_T, IsMethod>::createFromRuby()
  {
    // Does the From_Ruby instantiation work with Arg?
    if constexpr (std::is_constructible_v<From_Ruby<T>, Arg*>)
    {
      return From_Ruby<T>(&this->methodInfo_->arg(I));
    }
    else
    {
      return From_Ruby<T>();
    }
  }

  template<typename Function_T, bool IsMethod>
  To_Ruby<typename NativeFunction<Function_T, IsMethod>::Return_T> NativeFunction<Function_T, IsMethod>::createToRuby()
  {
    if (this->methodInfo_->returnInfo.isOwner())
    {
      if constexpr (std::is_constructible_v<To_Ruby<Return_T>, bool>)
      {
        return To_Ruby<Return_T>(true);
      }
      else
      {
        throw std::runtime_error("Type does not support taking onwership: " + typeName(typeid(Return_T)));
      }
    }
    // Is this type the same as Ruby's VALUE type? If so we need to tell the conversion
    // function if this is a VALUE or not
    else if (this->methodInfo_->returnInfo.getIsValue())
    {
      if constexpr (std::is_same_v<Return_T, VALUE>)
      {
        return To_Ruby<Return_T>(true);
      }
      else
      {
        throw std::runtime_error("Type does not support passing by VALUE: " + typeName(typeid(Return_T)));
      }
    }
    else
    {
      return To_Ruby<Return_T>();
    }
  }

  template<typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename NativeFunction<Function_T, IsMethod>::From_Ruby_Ts NativeFunction<Function_T, IsMethod>::createFromRuby(std::index_sequence<I...>& indices)
  {
    return std::make_tuple(createFromRuby<remove_cv_recursive_t<typename std::tuple_element<I, Arg_Ts>::type>, I>()...);
  }

  template<typename Function_T, bool IsMethod>
  std::vector<VALUE> NativeFunction<Function_T, IsMethod>::getRubyValues(int argc, VALUE* argv)
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
  typename NativeFunction<Function_T, IsMethod>::Arg_Ts NativeFunction<Function_T, IsMethod>::getNativeValues(std::vector<VALUE>& values,
     std::index_sequence<I...>& indices)
  {
    // Convert each Ruby value to its native value by calling the appropriate fromRuby instance.
    // Note that for fundamental types From_Ruby<Arg_Ts> will keep a copy of the native value
    // so it can be passed by reference or pointer to a native function.
    return std::forward_as_tuple(std::get<I>(this->fromRubys_).convert(values[I])...);
  }

  template<typename Function_T, bool IsMethod>
  typename NativeFunction<Function_T, IsMethod>::Self_T NativeFunction<Function_T, IsMethod>::getSelf(VALUE self)
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
      return From_Ruby<Self_T>().convert(self);
    }
  }

  template<typename Function_T, bool IsMethod>
  VALUE NativeFunction<Function_T, IsMethod>::invokeNativeFunction(Arg_Ts& nativeArgs)
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
      
      // Return the result
      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Function_T, bool IsMethod>
  VALUE NativeFunction<Function_T, IsMethod>::invokeNativeMethod(VALUE self, Arg_Ts& nativeArgs)
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
      Return_T nativeResult = (Return_T)std::apply(this->func_, selfAndNativeArgs);

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

      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Function_T, bool IsMethod>
  void NativeFunction<Function_T, IsMethod>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<VALUE>& rubyValues)
  {
    // Check function arguments
    Wrapper* selfWrapper = getWrapper(self);
    for (const Arg& arg : (*this->methodInfo_))
    {
      if (arg.isKeepAlive)
      {
        selfWrapper->addKeepAlive(rubyValues[arg.position]);
      }
    }

    // Check return value
    if (this->methodInfo_->returnInfo.isKeepAlive)
    {
      Wrapper* returnWrapper = getWrapper(returnValue);
      returnWrapper->addKeepAlive(self);
    }
  }

  template<typename Function_T, bool IsMethod>
  VALUE NativeFunction<Function_T, IsMethod>::operator()(int argc, VALUE* argv, VALUE self)
  {
    try
    {
      // Get the ruby values
      std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv);

      auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

      // Convert the Ruby values to native values
      Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

      // Now call the native method
      VALUE result = Qnil;
      if constexpr (std::is_same_v<Self_T, std::nullptr_t>)
      {
        result = this->invokeNativeFunction(nativeValues);
      }
      else
      {
        result = this->invokeNativeMethod(self, nativeValues);
      }

      // Check if any function arguments or return values need to have their lifetimes tied to the receiver
      this->checkKeepAlive(self, result, rubyValues);

      return result;
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
