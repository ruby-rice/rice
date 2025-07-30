#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Class_T, typename Method_T>
  void NativeMethod<Class_T, Method_T>::define(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo)
  {
    // Have we defined this method yet in Ruby?
    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create a NativeMethod instance and save it to the NativeRegistry. There may be multiple
    // NativeMethod instances for a specific method because C++ supports method overloading.
    NativeMethod_T* NativeMethod = new NativeMethod_T(klass, method_name, std::forward<Method_T>(method), methodInfo);
    std::unique_ptr<Native> native(NativeMethod);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Class_T, typename Method_T>
  NativeMethod<Class_T, Method_T>::NativeMethod(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo)
    : klass_(klass), method_name_(method_name), method_(method), methodInfo_(methodInfo), 
      toRuby_(methodInfo->returnInfo()), Native(Native::create_parameters<Arg_Ts>(methodInfo))
  {
  }

  template<typename Class_T, typename Method_T>
  template<std::size_t... I>
  std::vector<std::string> NativeMethod<Class_T, Method_T>::argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices)
  {
    std::vector<std::string> result;
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      result.push_back(parameter->cppTypeName());
    }
    return result;
  }

  template<typename Class_T, typename Method_T>
  std::string NativeMethod<Class_T, Method_T>::toString()
  {
    std::ostringstream result;

    result << cppClassName<Return_T>() << " ";
    
    if (!std::is_null_pointer_v<Receiver_T>)
    {
      result << cppClassName<Receiver_T>() << "::";
    }
    
    result << this->method_name_;

    result << "(";

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
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
    
  template<typename Class_T, typename Method_T>
  template<std::size_t... I>
  typename NativeMethod<Class_T, Method_T>::Arg_Ts NativeMethod<Class_T, Method_T>::getNativeValues(std::vector<std::optional<VALUE>>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ method. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    //return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Arg_Ts>, I>(values)...);
    return std::forward_as_tuple(
      (dynamic_cast<Parameter<std::tuple_element_t<I, Arg_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Class_T, typename Method_T>
  typename NativeMethod<Class_T, Method_T>::Receiver_T NativeMethod<Class_T, Method_T>::getReceiver(VALUE self)
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

  template<typename Class_T, typename Method_T>
  VALUE NativeMethod<Class_T, Method_T>::invoke(VALUE self, Arg_Ts&& nativeArgs)
  {
    Receiver_T receiver = this->getReceiver(self);
    auto selfAndNativeArgs = std::tuple_cat(std::forward_as_tuple(receiver), std::forward<Arg_Ts>(nativeArgs));

    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->method_, std::forward<decltype(selfAndNativeArgs)>(selfAndNativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = std::apply(this->method_, std::forward<decltype(selfAndNativeArgs)>(selfAndNativeArgs));

      // Special handling if the method returns self. If so we do not want
      // to create a new Ruby wrapper object and instead return self.
      if constexpr (std::is_same_v<intrinsic_type<Return_T>, intrinsic_type<Receiver_T>>)
      {
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

  template<typename Class_T, typename Method_T>
  void NativeMethod<Class_T, Method_T>::noWrapper(const VALUE klass, const std::string& wrapper)
  {
    std::stringstream message;

    message << "When calling the method `";
    message << this->method_name_;
    message << "' we could not find the wrapper for the '";
    message << rb_obj_classname(klass);
    message << "' ";
    message << wrapper;
    message << " type. You should not use keepAlive() on a Return or Arg that is a builtin Rice type.";

    throw std::runtime_error(message.str());
  }

  template<typename Class_T, typename Method_T>
  void NativeMethod<Class_T, Method_T>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues)
  {
    // Self will be Qnil for wrapped procs
    if (self == Qnil)
      return;

    // selfWrapper will be nullptr if this(self) is a builtin type and not an external(wrapped) type
    // it is highly unlikely that keepAlive is used in this case but we check anyway
    WrapperBase* selfWrapper = getWrapper(self);

    // Check method arguments
    for (const Arg& arg : (*this->methodInfo_))
    {
      if (arg.isKeepAlive())
      {
        if (selfWrapper == nullptr)
        {
          noWrapper(self, "self");
        }
        selfWrapper->addKeepAlive(rubyValues[arg.position].value());
      }
    }

    // Check return value
    if (this->methodInfo_->returnInfo()->isKeepAlive())
    {
      if (selfWrapper == nullptr)
      {
        noWrapper(self, "self");
      }

      // returnWrapper will be nullptr if returnValue is a built-in type and not an external(wrapped) type
      WrapperBase* returnWrapper = getWrapper(returnValue);
      if (returnWrapper == nullptr)
      {
        noWrapper(returnValue, "return");
      }
      returnWrapper->addKeepAlive(self);
    }
  }

  template<typename Class_T, typename Method_T>
  VALUE NativeMethod<Class_T, Method_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

    // Convert the Ruby values to native values
    Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    // Now call the native method
    VALUE result = this->invoke(self, std::forward<Arg_Ts>(nativeValues));

    // Check if any method arguments or return values need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, result, rubyValues);

    return result;
  }

  template<typename Class_T, typename Method_T>
  inline std::string NativeMethod<Class_T, Method_T>::name()
  {
    return this->method_name_;
  }

  template<typename Class_T, typename Method_T>
  inline NativeKind NativeMethod<Class_T, Method_T>::kind()
  {
    return NativeKind::Method;
  }

  template<typename Class_T, typename Method_T>
  inline std::string NativeMethod<Class_T, Method_T>::rubyReturnType()
  {
    if constexpr (std::is_fundamental_v<Return_T>)
    {
      return RubyType< detail::remove_cv_recursive_t<Return_T>>::name;
    }
    else
    {
      std::string module = rubyModuleName<Return_T>();
      if (module.empty())
      {
        return rubyClassName<Return_T>();
      }
      else
      {
        return module + "::" + rubyClassName<Return_T>();
      }
    }
  }
}
