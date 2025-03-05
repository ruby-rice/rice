#include <array>
#include <algorithm>
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
  Resolved NativeFunction<Class_T, Function_T, IsMethod>::matches(int argc, const VALUE* argv, VALUE self)
  {
    // Return false if Ruby provided more arguments than the C++ method takes
    if (argc > arity)
      return Resolved{ Convertible::None, 0, this };

    Resolved result { Convertible::Exact, 1, this };

    MethodInfo* methodInfo = this->methodInfo_.get();
    int index = 0;

    std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv, false);

    // Loop over each FromRuby instance
    for_each_tuple(this->fromRubys_,
      [&](auto& fromRuby)
      {
        Convertible convertible = Convertible::None;

        const Arg* arg = methodInfo->arg(index);

        // Is a VALUE being passed directly to C++ ?
        if (arg->isValue() && index < rubyValues.size())
        {
          convertible = Convertible::Exact;
        }
        // If index is less than argc then check with FromRuby if the VALUE is convertible
        // to C++.
        else if (index < rubyValues.size())
        {
          VALUE value = rubyValues[index];
          convertible = fromRuby.is_convertible(value);
        }
        // Last check if a default value has been set
        else if (arg->hasDefaultValue())
        {
          convertible = Convertible::Exact;
        }

        result.convertible = result.convertible & convertible;

        index++;
      });

    if constexpr (arity > 0)
      result.parameterMatch = rubyValues.size() / (double)arity;

    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  std::vector<VALUE> NativeFunction<Class_T, Function_T, IsMethod>::getRubyValues(int argc, const VALUE* argv, bool validate)
  {
    std::vector<VALUE> result;

    // Keyword handling
    if (rb_keyword_given_p())
    {
      // Keywords are stored in the last element in a hash
      int actualArgc = argc - 1;

      VALUE value = argv[actualArgc];
      Hash keywords(value);

      result.resize(actualArgc + keywords.size());

      // Copy over leading arguments
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
      std::copy(argv, argv + argc, std::back_inserter(result));
    }
     
    // Block handling. If we find a block and the last parameter is missing then
    // set it to the block
    if (rb_block_given_p() && result.size() < std::tuple_size_v<Arg_Ts>)
    {
      VALUE proc = rb_block_proc();
      result.push_back(proc);
    }

    if (validate)
    {
      this->methodInfo_->verifyArgCount(result.size());
    }
    
    return result;
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<typename Arg_T, int I>
  Arg_T NativeFunction<Class_T, Function_T, IsMethod>::getNativeValue(std::vector<VALUE>& values)
  {
    /* In general the compiler will convert T to const T, but that does not work for converting
       T** to const T** (see see https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion)
       which comes up in the OpenCV bindings.
     
       An alternative solution is updating From_Ruby#convert to become a templated function that specifies
       the return type. That works but requires a lot more code changes for this one case and is not 
       backwards compatible. */

    // If the user did provide a value assume Qnil
    VALUE value = I < values.size() ? values[I] : Qnil;

    if constexpr (is_pointer_pointer_v<Arg_T> && !std::is_convertible_v<remove_cv_recursive_t<Arg_T>, Arg_T>)
    {
      return (Arg_T)std::get<I>(this->fromRubys_).convert(value);
    }
    else
    {
      return std::get<I>(this->fromRubys_).convert(value);
    }
  }

  template<typename Class_T, typename Function_T, bool IsMethod>
  template<std::size_t... I>
  typename NativeFunction<Class_T, Function_T, IsMethod>::Arg_Ts NativeFunction<Class_T, Function_T, IsMethod>::getNativeValues(std::vector<VALUE>& values,
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
      return this->toRuby_.convert(nativeResult);
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

      return this->toRuby_.convert(nativeResult);
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
  void NativeFunction<Class_T, Function_T, IsMethod>::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<VALUE>& rubyValues)
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
        selfWrapper->addKeepAlive(rubyValues[arg.position]);
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
  VALUE NativeFunction<Class_T, Function_T, IsMethod>::operator()(int argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<VALUE> rubyValues = this->getRubyValues(argc, argv, true);

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
