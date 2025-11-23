#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Class_T, typename Method_T, bool NoGVL>
  template<typename ...Arg_Ts>
  void NativeMethod<Class_T, Method_T, NoGVL>::define(VALUE klass, std::string method_name, Method_T method, Arg_Ts&& ...args)
  {
    // Verify return type
    using Arg_Tuple = std::tuple<Arg_Ts...>;
    constexpr bool isBuffer = tuple_element_index_v<Arg_Tuple, ReturnBuffer> < std::tuple_size_v<Arg_Tuple>;
    Native::verify_type<Return_T, isBuffer>();

    // Have we defined this method yet in Ruby?
    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create method parameters - this will also validate their types
    std::vector<std::unique_ptr<ParameterAbstract>> parameters = Native::create_parameters<Parameter_Ts>(args...);

    // Create return info
    std::unique_ptr<Return> returnInfo = Native::create_return<Arg_Ts...>(args...);
    
    // Create native method
    NativeMethod_T* nativeMethod = new NativeMethod_T(klass, method_name, std::forward<Method_T>(method), std::move(returnInfo), std::move(parameters));
    std::unique_ptr<Native> native(nativeMethod);

    // Register the native method
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  NativeMethod<Class_T, Method_T, NoGVL>::NativeMethod(VALUE klass, std::string method_name, Method_T method, std::unique_ptr<Return>&& returnInfo, std::vector<std::unique_ptr<ParameterAbstract>>&& parameters)
    : Native(std::move(returnInfo), std::move(parameters)),
      klass_(klass), method_name_(method_name), method_(method), toRuby_(returnInfo_.get())
  {
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  template<std::size_t... I>
  std::vector<std::string> NativeMethod<Class_T, Method_T, NoGVL>::argTypeNames(std::ostringstream& stream, const std::index_sequence<I...>& indices)
  {
    std::vector<std::string> result;
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      result.push_back(parameter->cppTypeName());
    }
    return result;
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  std::string NativeMethod<Class_T, Method_T, NoGVL>::toString()
  {
    std::ostringstream result;

    detail::TypeMapper<Return_T> typeReturnMapper;
    result << typeReturnMapper.simplifiedName() << " ";
    
    if (!std::is_null_pointer_v<Receiver_T>)
    {
      detail::TypeMapper<Receiver_T> typeReceiverMapper;
      result << typeReceiverMapper.simplifiedName() << "::";
    }
    
    result << this->method_name_;

    result << "(";

    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Ts>>{};
    std::vector<std::string> argTypeNames = this->argTypeNames(result, indices);
    for (size_t i = 0; i < argTypeNames.size(); i++)
    {
      result << argTypeNames[i];
      if (i < argTypeNames.size() - 1)
        result << ", ";
    }
    result << ")";
    return result.str();
  }
    
  template<typename Class_T, typename Method_T, bool NoGVL>
  template<std::size_t... I>
  typename NativeMethod<Class_T, Method_T, NoGVL>::Apply_Args_T NativeMethod<Class_T, Method_T, NoGVL>::getNativeValues(VALUE self, std::vector<std::optional<VALUE>>& values, const std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Parameter_Ts) required by the C++ method. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    return std::forward_as_tuple(this->getReceiver(self),
      (dynamic_cast<Parameter<std::tuple_element_t<I, Parameter_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  typename NativeMethod<Class_T, Method_T, NoGVL>::Receiver_T NativeMethod<Class_T, Method_T, NoGVL>::getReceiver(VALUE self)
  {
    // Self parameter is a Ruby VALUE so no conversion is needed
    if constexpr (std::is_same_v<Receiver_T, VALUE>)
    {
      return self;
    }
    /* This case happens when a class wrapped by Rice is calling a method
       defined on an ancestor class. For example, the std::map size method
       is defined on _Tree not map. Rice needs to know the actual type
       that was wrapped so it can correctly extract the C++ object from 
       the Ruby object. */
    else if constexpr (!std::is_same_v<intrinsic_type<Receiver_T>, Class_T> && 
                        std::is_base_of_v<intrinsic_type<Receiver_T>, Class_T> &&
                        std::is_pointer_v<Receiver_T>)
    {
      Class_T* instance = From_Ruby<Class_T*>().convert(self);
      return dynamic_cast<Receiver_T>(instance);
    }
    else if constexpr (!std::is_same_v<intrinsic_type<Receiver_T>, Class_T> &&
                        std::is_base_of_v<intrinsic_type<Receiver_T>, Class_T> &&
                        std::is_reference_v<Receiver_T>)
    {
      Class_T& instance = From_Ruby<Class_T&>().convert(self);
      return dynamic_cast<Receiver_T>(instance);
    }
    // Self parameter could be derived from Object or it is an C++ instance and
    // needs to be unwrapped from Ruby
    else
    {
      return From_Ruby<Receiver_T>().convert(self);
    }
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  inline VALUE NativeMethod<Class_T, Method_T, NoGVL>::invoke(VALUE self, Apply_Args_T&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->method_, std::forward<Apply_Args_T>(nativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = std::apply(this->method_, std::forward<Apply_Args_T>(nativeArgs));

      // Special handling if the method returns self. If so we do not want
      // to create a new Ruby wrapper object and instead return self.
      if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Receiver_T>>)
      {
				Receiver_T receiver = std::get<0>(nativeArgs);

        if constexpr (std::is_pointer_v<Return_T> && std::is_pointer_v<Receiver_T>)
        {
          if (nativeResult == receiver)
            return self;
        }
        else if constexpr (std::is_pointer_v<Return_T> && std::is_reference_v<Receiver_T>)
        {
          if (nativeResult == &receiver)
            return self;
        }
        else if constexpr (std::is_reference_v<Return_T> && std::is_pointer_v<Receiver_T>)
        {
          if (&nativeResult == receiver)
            return self;
        }
        else if constexpr (std::is_reference_v<Return_T> && std::is_reference_v<Receiver_T>)
        {
          if (&nativeResult == &receiver)
            return self;
        }
      }

      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  inline VALUE NativeMethod<Class_T, Method_T, NoGVL>::invokeNoGVL(VALUE self, Apply_Args_T&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      no_gvl(this->method_, std::forward<Apply_Args_T>(nativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = no_gvl(this->method_, std::forward<Apply_Args_T>(nativeArgs));

      // Special handling if the method returns self. If so we do not want
      // to create a new Ruby wrapper object and instead return self.
      if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Receiver_T>>)
      {
        Receiver_T receiver = std::get<0>(nativeArgs);

        if constexpr (std::is_pointer_v<Return_T> && std::is_pointer_v<Receiver_T>)
        {
          if (nativeResult == receiver)
            return self;
        }
        else if constexpr (std::is_pointer_v<Return_T> && std::is_reference_v<Receiver_T>)
        {
          if (nativeResult == &receiver)
            return self;
        }
        else if constexpr (std::is_reference_v<Return_T> && std::is_pointer_v<Receiver_T>)
        {
          if (&nativeResult == receiver)
            return self;
        }
        else if constexpr (std::is_reference_v<Return_T> && std::is_reference_v<Receiver_T>)
        {
          if (&nativeResult == &receiver)
            return self;
        }
      }

      return this->toRuby_.convert(nativeResult);
    }
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  VALUE NativeMethod<Class_T, Method_T, NoGVL>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);
    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Ts>>{};
    Apply_Args_T nativeArgs = this->getNativeValues(self, rubyValues, indices);

    VALUE result = Qnil;

    if constexpr (NoGVL)
    {
      result = this->invokeNoGVL(self, std::forward<Apply_Args_T>(nativeArgs));
    }
    else
    {
      result = this->invoke(self, std::forward<Apply_Args_T>(nativeArgs));
    }

    // Check if any method arguments or return values need to have their lifetimes tied to the receiver
    Native_checkKeepAlive(self, result, rubyValues, this->method_name_, this);

    return result;
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  inline std::string NativeMethod<Class_T, Method_T, NoGVL>::name()
  {
    return this->method_name_;
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  inline NativeKind NativeMethod<Class_T, Method_T, NoGVL>::kind()
  {
    return NativeKind::Method;
  }

  template<typename Class_T, typename Method_T, bool NoGVL>
  inline VALUE NativeMethod<Class_T, Method_T, NoGVL>::returnKlass()
  {
    // Check if an array is being returned
    bool isBuffer = dynamic_cast<ReturnBuffer*>(this->returnInfo_.get()) ? true : false;
    if (isBuffer)
    {
      TypeMapper<Pointer<Return_T>> typeMapper;
      return typeMapper.rubyKlass();
    }
    else
    {
      TypeMapper<Return_T> typeMapper;
      return typeMapper.rubyKlass();
    }
  }
}
