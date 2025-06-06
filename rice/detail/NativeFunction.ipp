#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>

namespace Rice::detail
{
  template<typename Class_T, typename Function_T, bool IsMethod>
  void NativeFunction<Class_T, Function_T, IsMethod>::define(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo)
  {
    // Have we defined this method yet in Ruby?
    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create a NativeFunction instance and save it to the NativeRegistry. There may be multiple
    // NativeFunction instances for a specific method because C++ supports method overloading.
    NativeFunction_T* nativeFunction = new NativeFunction_T(klass, method_name, std::forward<Function_T>(function), methodInfo);
    std::unique_ptr<Native> native(nativeFunction);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  // Ruby calls this method when invoking a proc that was defined as a C++ function
  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::procEntry(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    return cpp_protect([&]
    {
      NativeFunction_T * native = (NativeFunction_T*)callback_arg;
      return (*native)(argc, argv, Qnil);
    });
  }

  // Ruby calls this method if an instance f a NativeFunction is owned by a Ruby proc. That happens when C++
  // returns a function back to Ruby
  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    NativeFunction_T* native = (NativeFunction_T*)callback_arg;
    delete native;
    return Qnil;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  NativeFunction<Class_T, Function_T, IsMethod>::NativeFunction(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo)
    : klass_(klass), method_name_(method_name), function_(function), methodInfo_(methodInfo)
  {
    // Create a tuple of NativeArgs that will convert the Ruby values to native values. For 
    // builtin types NativeArgs will keep a copy of the native value so that it 
    // can be passed by reference or pointer to the native function. For non-builtin types
    // it will just pass the value through.
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    this->fromRubys_ = this->createFromRuby(indices);

    this->toRuby_ = this->createToRuby();
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  NativeFunction<Class_T, Function_T, IsMethod>::NativeFunction(Function_T function)
    : NativeFunction(Qnil, "", function, new MethodInfo(arity))
  {
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  std::vector<std::string> NativeFunction<Class_T, Function_T, IsMethod>::argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices)
  {
    std::vector<std::string> typeNames;
    (typeNames.push_back(cppClassName(typeName(typeid(typename std::tuple_element<I, Arg_Ts>::type)))), ...);
    return typeNames;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  std::string NativeFunction<Class_T, Function_T, IsMethod>::toString()
  {
    std::ostringstream result;

    result << cppClassName(typeName(typeid(Return_T))) << " ";
    
    if (!std::is_null_pointer_v<Receiver_T>)
    {
      result << cppClassName(typeName(typeid(Receiver_T))) << "::";
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
    
  template<typename Class_T, typename Function_T, bool IsMethod>
  To_Ruby<typename NativeFunction<Class_T, Function_T, IsMethod>::To_Ruby_T> NativeFunction<Class_T, Function_T, IsMethod>::createToRuby()
  {
    // Does the From_Ruby instantiation work with ReturnInfo?
    if constexpr (std::is_constructible_v<To_Ruby<To_Ruby_T>, Return*>)
    {
      return To_Ruby<To_Ruby_T>(&this->methodInfo_->returnInfo);
    }
    else
    {
      return To_Ruby<To_Ruby_T>();
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<typename T, std::size_t I>
  From_Ruby<T> NativeFunction<Class_T, Function_T, IsMethod>::createFromRuby()
  {
    // Does the From_Ruby instantiation work with Arg?
    if constexpr (std::is_constructible_v<From_Ruby<T>, Arg*>)
    {
      return From_Ruby<T>(this->methodInfo_->arg(I));
    }
    else
    {
      return From_Ruby<T>();
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename NativeFunction<Class_T, Function_T, IsMethod>::From_Ruby_Args_Ts NativeFunction<Class_T, Function_T, IsMethod>::createFromRuby(std::index_sequence<I...>& indices)
  {
    return std::make_tuple(createFromRuby<remove_cv_recursive_t<typename std::tuple_element<I, Arg_Ts>::type>, I>()...);
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<int I>
  Convertible NativeFunction<Class_T, Function_T, IsMethod>::matchParameter(std::vector<std::optional<VALUE>>& values)
  {
    Convertible result = Convertible::None;
    MethodInfo* methodInfo = this->methodInfo_.get();
    const Arg* arg = methodInfo->arg(I);
    std::optional<VALUE> value = values[I];

    // Is a VALUE being passed directly to C++ ?
    if (value.has_value())
    {
      if (arg->isValue())
      {
        result = Convertible::Exact;
      }
      // If index is less than argc then check with FromRuby if the VALUE is convertible
      // to C++.
      else
      {
        VALUE value = values[I].value();
        auto fromRuby = std::get<I>(this->fromRubys_);
        result = fromRuby.is_convertible(value);

        // If this is an exact match check if the const-ness of the value and the parameter match
        if (result == Convertible::Exact && rb_type(value) == RUBY_T_DATA)
        {
          // Check the constness of the Ruby wrapped value and the parameter
          WrapperBase* wrapper = getWrapper(value);
          using Parameter_T = std::tuple_element_t<I, Arg_Ts>;

          // Do not send a const value to a non-const parameter
          if (wrapper->isConst() && !is_const_any_v<Parameter_T>)
          {
            result = Convertible::None;
          }
          // It is ok to send a non-const value to a const parameter but
          // prefer non-const to non-const by slighly decreasing the convertible value
          else if (!wrapper->isConst() && is_const_any_v<Parameter_T>)
          {
            result = Convertible::Const;
          }
        }
      }
    }
    // Last check if a default value has been set
    else if (arg->hasDefaultValue())
    {
      result = Convertible::Exact;
    }

    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  Convertible NativeFunction<Class_T, Function_T, IsMethod>::matchParameters(std::vector<std::optional<VALUE>>& values,
    std::index_sequence<I...>& indices)
  {
    Convertible result = Convertible::Exact;
    ((result = result & this->matchParameter<I>(values)), ...);
    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  Resolved NativeFunction<Class_T, Function_T, IsMethod>::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    // Return false if Ruby provided more arguments than the C++ method takes
    if (argc > arity)
      return Resolved{ Convertible::None, 0, this };

    Resolved result { Convertible::Exact, 1, this };

    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, false);
    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};
    result.convertible = this->matchParameters(rubyValues, indices);

    if constexpr (arity > 0)
    {
      int providedValues = std::count_if(rubyValues.begin(), rubyValues.end(), [](std::optional<VALUE>& value)
        {
          return value.has_value();
        });

      result.parameterMatch = providedValues / (double)arity;
    }
    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  std::vector<std::optional<VALUE>> NativeFunction<Class_T, Function_T, IsMethod>::getRubyValues(size_t argc, const VALUE* argv, bool validate)
  {
#undef max
    int size = std::max((size_t)arity, (size_t)argc);
    std::vector<std::optional<VALUE>> result(size);

    // Keyword handling
    if (rb_keyword_given_p())
    {
      // Keywords are stored in the last element in a hash
      int actualArgc = argc - 1;

      VALUE value = argv[actualArgc];
      Hash keywords(value);

      // Copy over leading non-keyword arguments
      for (int i = 0; i < actualArgc; i++)
      {
        result[i] = argv[i];
      }

      // Copy over keyword arguments
      for (auto pair : keywords)
      {
        Symbol key(pair.first);
        const Arg* arg = this->methodInfo_->arg(key.str());
        if (!arg)
        {
          throw std::invalid_argument("Unknown keyword: " + key.str());
        }
        result[arg->position] = pair.second.value();
      }
    }
    else
    {
      std::copy(argv, argv + argc, result.begin());
    }
     
    // Block handling. If we find a block and the last parameter is missing then
    // set it to the block
    if (rb_block_given_p() && result.size() > 0 && !result.back().has_value())
    {
      VALUE proc = rb_block_proc();
      result.back() = proc;
    }

    if (validate)
    {
      // Protect against user sending too many arguments
      if (argc > arity)
      {
        std::string message = "wrong number of arguments (given " +
          std::to_string(argc) + ", expected " + std::to_string(arity) + ")";
        throw std::invalid_argument(message);
      }

      for (size_t i=0; i<result.size(); i++)
      {
        std::optional<VALUE> value = result[i];
        Arg* arg = this->methodInfo_->arg(i);

        if (!arg->hasDefaultValue() && !value.has_value())
        {
          std::string message;
          message = "Missing argument. Name: " + arg->name + ". Index: " + std::to_string(arg->position) + ".";
          throw std::invalid_argument(message);
        }
      }
    }
    
    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<typename Arg_T, int I>
  Arg_T NativeFunction<Class_T, Function_T, IsMethod>::getNativeValue(std::vector<std::optional<VALUE>>& values)
  {
    /* In general the compiler will convert T to const T, but that does not work for converting
       T** to const T** (see see https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion)
       which comes up in the OpenCV bindings.
     
       An alternative solution is updating From_Ruby#convert to become a templated function that specifies
       the return type. That works but requires a lot more code changes for this one case and is not 
       backwards compatible. */

    std::optional<VALUE> value = values[I];
    Arg* arg = this->methodInfo_->arg(I);

    if constexpr (is_pointer_pointer_v<Arg_T> && !std::is_convertible_v<remove_cv_recursive_t<Arg_T>, Arg_T>)
    {
      return (Arg_T)std::get<I>(this->fromRubys_).convert(value.value());
    }
    else if (value.has_value())
    {
      return std::get<I>(this->fromRubys_).convert(value.value());
    }
    else if constexpr (std::is_constructible_v<std::remove_cv_t<Arg_T>, std::remove_cv_t<std::remove_reference_t<Arg_T>>&>)
    {
      if (arg->hasDefaultValue())
      {
        return arg->defaultValue<Arg_T>();
      }
    }

    throw std::invalid_argument("Could not convert Rubyy value");
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename NativeFunction<Class_T, Function_T, IsMethod>::Arg_Ts NativeFunction<Class_T, Function_T, IsMethod>::getNativeValues(std::vector<std::optional<VALUE>>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in getNativeValue method. */
    return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Arg_Ts>, I>(values)...);
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  typename NativeFunction<Class_T, Function_T, IsMethod>::Receiver_T NativeFunction<Class_T, Function_T, IsMethod>::getReceiver(VALUE self)
  {
    // There is no self parameter
    if constexpr (std::is_same_v<Receiver_T, std::nullptr_t>)
    {
      return nullptr;
    }
    // Self parameter is a Ruby VALUE so no conversion is needed
    else if constexpr (std::is_same_v<Receiver_T, VALUE>)
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

  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::invokeNativeFunction(Arg_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->function_, std::forward<Arg_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = std::apply(this->function_, std::forward<Arg_Ts>(nativeArgs));

      // Return the result
      return this->toRuby_.convert(std::forward<Return_T>(nativeResult));
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::invokeNativeMethod(VALUE self, Arg_Ts&& nativeArgs)
  {
    Receiver_T receiver = this->getReceiver(self);
    auto selfAndNativeArgs = std::tuple_cat(std::forward_as_tuple(receiver), std::forward<Arg_Ts>(nativeArgs));

    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->function_, std::forward<decltype(selfAndNativeArgs)>(selfAndNativeArgs));
      return Qnil;
    }
    else
    {
      Return_T nativeResult = std::apply(this->function_, std::forward<decltype(selfAndNativeArgs)>(selfAndNativeArgs));

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

      return this->toRuby_.convert(std::forward<Return_T>(nativeResult));
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  void NativeFunction<Class_T, Function_T, IsMethod>::noWrapper(const VALUE klass, const std::string& wrapper)
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

  template<typename Class_T, typename Function_T, bool IsMethod>
  void NativeFunction<Class_T, Function_T, IsMethod>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues)
  {
    // Self will be Qnil for wrapped procs
    if (self == Qnil)
      return;

    // selfWrapper will be nullptr if this(self) is a builtin type and not an external(wrapped) type
    // it is highly unlikely that keepAlive is used in this case but we check anyway
    WrapperBase* selfWrapper = getWrapper(self);

    // Check function arguments
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
    if (this->methodInfo_->returnInfo.isKeepAlive())
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

  template<typename Class_T, typename Function_T, bool IsMethod>
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);

    auto indices = std::make_index_sequence<std::tuple_size_v<Arg_Ts>>{};

    // Convert the Ruby values to native values
    Arg_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    // Now call the native method
    VALUE result = Qnil;
    if constexpr (std::is_same_v<Receiver_T, std::nullptr_t>)
    {
      result = this->invokeNativeFunction(std::forward<Arg_Ts>(nativeValues));
    }
    else
    {
      result = this->invokeNativeMethod(self, std::forward<Arg_Ts>(nativeValues));
    }

    // Check if any function arguments or return values need to have their lifetimes tied to the receiver
    this->checkKeepAlive(self, result, rubyValues);

    return result;
  }
}
