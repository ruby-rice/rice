RICE_DETAIL_BEGIN_NAMESPACE
  inline bool Resolved::operator<(Resolved other)
  {
    if (this->convertible != other.convertible)
    {
      return this->convertible < other.convertible;
    }
    else
    {
      return this->parameterMatch < other.parameterMatch;
    }
  }

  inline bool Resolved::operator>(Resolved other)
  {
    if (this->convertible != other.convertible)
    {
      return this->convertible > other.convertible;
    }
    else
    {
      return this->parameterMatch > other.parameterMatch;
    }
  }

  inline VALUE Native::resolve(int argc, VALUE* argv, VALUE self)
  {
    /* This method is called from Ruby and is responsible for determining the correct
       Native object (ie, NativeFunction, NativeIterator, NativeAttributeGet and 
       NativeAttributeSet) that shoudl be used to invoke the underlying C++ code.
       Most of the time there will be a single Native object registered for a C++ function,
       method, constructor, iterator or attribute. However, there can be multiple Natives 
       when a C++ function/method/construtor is overloaded.

       In that case, the code iterates over each Native and calls its matches method. The matches
       method returns a Resolved object which includes a Convertible field and parameterMatch field.
       The Convertible field is an enum that specifies if the types of the values supplied by Ruby
       match the types of the C++ function parameters. Allowed values include  can be Exact (example Ruby into to C++ int),
       TypeCast (example Ruby into to C++ float) or None (cannot be converted). 

       The parameterMatch field is simply the number or arguments provided by Ruby divided by the
       number of arguments required by C++. These numbers can be different because C++ method 
       parameters can have default values.

       Taking these two values into account, the method sorts the Natives and picks the one with the 
       highest score (Convertible::Exact and 1.0 for parameterMatch). Thus given these two C++ functions:

       void some_method(int a);
       void some_mtehod(int a, float b = 2.0).

       A call from ruby of some_method(1) will exactly match both signatures, but the first one 
       will be chosen because the parameterMatch will be 1.0 for the first overload but 0.5
       for the second. */

    Native* native = nullptr;

    ID methodId;
    VALUE klass;
    if (!rb_frame_method_id_and_class(&methodId, &klass))
    {
      rb_raise(rb_eRuntimeError, "Cannot get method id and class for function");
    }

    // Execute the function but make sure to catch any C++ exceptions!
    return cpp_protect([&]()
    {
      const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, methodId);

      if (natives.size() == 1)
      {
        native = natives.front().get();
      }
      else if (natives.size() == 0)
      {
        Identifier identifier(methodId);
        rb_enc_raise(rb_utf8_encoding(), rb_eArgError, "Could not find method call for %s#%s", rb_class2name(klass), identifier.c_str());
      }
      else
      {
        Identifier identifier(methodId);

        // Loop over every native to see how well they match the Ruby parameters
        std::vector<Resolved> resolves;
        std::transform(natives.begin(), natives.end(), 
          std::back_inserter(resolves), 
          [&](const std::unique_ptr<Native>& native)
          {
            return native->matches(argc, argv);
          });

        // Now sort from best to worst
        std::sort(resolves.begin(), resolves.end(), std::greater{});

        // Get the first one
        Resolved resolved = resolves.front();

        // Was there more than one match?
        /*size_t count = std::count_if(resolves.begin(), resolves.end(),
          [&resolved](Resolved& element)
          {
            return resolved.convertible == element.convertible;
          });

        if (count > 1)
        {
          std::ostringstream message;
          message << "Could not resolve method call for %s#%s" << "\n"
                  << "  %d overloaded functions matched based on the types of Ruby parameters provided:";

          for (int i = 0; i < count; i++)
          {
            message << "\n     " << resolves[i].native->toString();
          }
          
          rb_raise(rb_eArgError, message.str().c_str(), rb_class2name(klass), identifier.c_str(), count);
        }*/

        // Did it match?
        if (resolved.convertible != Convertible::None)
        {
          native = resolved.native;
        }
        else
        {
          // Special case == to make the RubyMine debugger work. It calls == with a Module as
          // the other argument, thus breaking if C++ operator== is implemented.
          if (identifier.str() == "==")
          {
            return detail::protect(rb_call_super, argc, argv);
          }
          else
          {
            std::ostringstream message;
            message << "Could not resolve method call for %s#%s" << "\n"
                    << "  %d overload(s) were evaluated based on the types of Ruby parameters provided:";

            for (Resolved& resolve: resolves)
            {
              message << "\n     " << resolve.native->toString();
            }

            rb_enc_raise(rb_utf8_encoding(), rb_eArgError, message.str().c_str(), rb_class2name(klass), identifier.c_str(), natives.size());
          }
        }
      }

      // Call the C++ function
      return (*native)(argc, argv, self);
    });
  }

  inline Native::Native(std::string name) :
    name_(name)
  {
  }

  inline Native::Native(std::string name,std::unique_ptr<Return>&& returnInfo) :
    name_(name), returnInfo_(std::move(returnInfo))
  {
  }

  inline Native::Native(std::string name, std::unique_ptr<Return>&& returnInfo, std::vector<std::unique_ptr<ParameterAbstract>>&& parameters) :
    name_(name), returnInfo_(std::move(returnInfo)), parameters_(std::move(parameters))
  {
  }

  inline std::string Native::name()
  {
    return this->name_;
  }
  inline ParameterAbstract* Native::getParameterByName(std::string name)
  {
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      if (parameter->arg()->name == name)
      {
        return parameter.get();
      }
    }

    return nullptr;
  }

  inline void Native::checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues)
  {
    // Check function arguments
    for (size_t i = 0; i < this->parameters_.size(); i++)
    {
      Arg* arg = parameters_[i]->arg();
      if (arg->isKeepAlive())
      {
        static WrapperBase* selfWrapper = getWrapper(self);
        selfWrapper->addKeepAlive(rubyValues[i].value());
      }
    }

    // Check return value
    if (this->returnInfo_->isKeepAlive())
    {
      WrapperBase* returnWrapper = getWrapper(returnValue);
      returnWrapper->addKeepAlive(self);
    }
  }

  // -----------  Type Checking ----------------
  template<typename T, bool isBuffer>
  inline void Native::verify_type()
  {
    using Base_T = std::remove_pointer_t<remove_cv_recursive_t<T>>;

    detail::verifyType<T>();

    if constexpr (std::is_pointer_v<T> && std::is_fundamental_v<std::remove_pointer_t<T>>)
    {
      Type<Pointer<Base_T>>::verify();
      Type<Buffer<Base_T>>::verify();
    }
    else if constexpr (std::is_pointer_v<Base_T>)
    {
      Type<Pointer<Base_T>>::verify();
      Type<Buffer<Base_T>>::verify();
    }
    else if constexpr (std::is_array_v<T>)
    {
      Type<Pointer<std::remove_extent_t<remove_cv_recursive_t<T>>>>::verify();
      Type<Buffer<std::remove_extent_t<remove_cv_recursive_t<T>>>>::verify();
    }
    else if constexpr (isBuffer)
    {
      Type<Pointer<Base_T>>::verify();
      Type<Buffer<Base_T>>::verify();
    }
  }

  template<typename Parameter_Tuple, typename Arg_Tuple, size_t I>
  inline void Native::verify_parameter()
  {
    using Param_T = std::tuple_element_t<I, Parameter_Tuple>;
    using Arg_T = std::tuple_element_t<I, Arg_Tuple>;
    if constexpr (std::is_same_v<ArgBuffer, std::decay_t<Arg_T>>)
    {
      verify_type<Param_T, true>();
    }
    else
    {
      verify_type<Param_T, false>();
    }
  };

  template<typename Parameter_Tuple, typename Arg_Tuple, std::size_t ...Indices>
  inline void Native::create_parameters_impl(std::vector<std::unique_ptr<ParameterAbstract>>& parameters, std::index_sequence<Indices...>, std::vector<std::unique_ptr<Arg>>&& args)
  {
    // Verify parameter types
    (verify_parameter<Parameter_Tuple, Arg_Tuple, Indices>(), ...);

    // Create parameters
    (parameters.push_back(std::move(std::make_unique<
      Parameter<std::tuple_element_t<Indices, Parameter_Tuple>>>(std::move(args[Indices])))), ...);
  }

  template<typename Parameter_Tuple, typename... Arg_Ts>
  inline std::vector<std::unique_ptr<ParameterAbstract>> Native::create_parameters(Arg_Ts&& ...args)
  {
    std::vector<std::unique_ptr<ParameterAbstract>> result;

    // Extract Arg and ArgBuffer from Arg_Ts and then pad Arg to match the size of Parameter_Tuple
    using ArgsBaseTuple = tuple_filter_types_t<std::tuple<Arg_Ts...>, Arg, ArgBuffer>;

    // Diff can be less than zero so it has to be signed! This happens when define_method is called with a self
    // parameter and specifies one or more Args (usually to call Arg("self).setValue()). 
    // In that case the self parameter is considered Class_T and there are no arguments.
    constexpr long diff = (long)std::tuple_size_v<Parameter_Tuple> - (long)std::tuple_size_v<ArgsBaseTuple>;
    using ArgsTuple = tuple_pad_type_t<ArgsBaseTuple, Arg, diff < 0 ? 0 : diff>;
    
    // Now play the same game but with the tuple values instead of types
    std::vector<std::unique_ptr<Arg>> argsVector;

    // Loop over each arg with an anonymous lambda
    ([&]
      {
        using Arg_T = std::decay_t<Arg_Ts>;

        if constexpr (std::is_same_v<Arg, Arg_T> || std::is_same_v<ArgBuffer, Arg_T>)
        {
          argsVector.emplace_back(std::make_unique<Arg_T>(args));
        }
      }(), ...);

    // Fill in missing args
    for (size_t i = argsVector.size(); i < std::tuple_size_v<Parameter_Tuple>; i++)
    {
      argsVector.emplace_back(std::make_unique<Arg>("arg_" + std::to_string(i)));
    }

    // TODO - there has to be a better way!
    for (size_t i = 0; i < argsVector.size(); i++)
    {
      std::unique_ptr<Arg>& arg = argsVector[i];
      arg->position = (int32_t)i;
    }

    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Tuple>>{};
    Native::create_parameters_impl<Parameter_Tuple, ArgsTuple>(result, indices, std::move(argsVector));
    return result;
  }

  template<typename... Arg_Ts>
  inline std::unique_ptr<Return> Native::create_return(Arg_Ts& ...args)
  {
    using Arg_Tuple = std::tuple<Arg_Ts...>;

    constexpr std::size_t index = tuple_element_index_v<Arg_Tuple, Return, ReturnBuffer>;

    std::unique_ptr<Return> result;

    if constexpr (index < std::tuple_size_v<Arg_Tuple>)
    {
      using Return_T_Local = std::decay_t<std::tuple_element_t<index, Arg_Tuple>>;
      const Return_T_Local& returnInfo = std::get<index>(std::forward_as_tuple(std::forward<Arg_Ts>(args)...));
      result = std::make_unique<Return_T_Local>(returnInfo);
    }
    else
    {
      result = std::make_unique<Return>();
    }

    return result;
  }

  inline std::vector<std::optional<VALUE>> Native::getRubyValues(size_t argc, const VALUE* argv, bool validate)
  {
#undef max
    size_t size = std::max(this->parameters_.size(), argc);
    std::vector<std::optional<VALUE>> result(size);

    // Keyword handling
    if (rb_keyword_given_p())
    {
      // Keywords are stored in the last element in a hash
      size_t actualArgc = argc - 1;

      VALUE value = argv[actualArgc];
      Hash keywords(value);

      // Copy over leading non-keyword arguments
      for (size_t i = 0; i < actualArgc; i++)
      {
        result[i] = argv[i];
      }

      // Copy over keyword arguments
      for (auto pair : keywords)
      {
        Symbol key(pair.first);
        ParameterAbstract* parameter = this->getParameterByName(key.str());
        if (!parameter)
        {
          throw std::invalid_argument("Unknown keyword: " + key.str());
        }

        const Arg* arg = parameter->arg();

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
      if (argc > this->parameters_.size())
      {
        std::string message = "wrong number of arguments (given " +
          std::to_string(argc) + ", expected " + std::to_string(this->parameters_.size()) + ")";
        throw std::invalid_argument(message);
      }

      for (size_t i = 0; i < result.size(); i++)
      {
        std::optional<VALUE> value = result[i];
        ParameterAbstract* parameter = this->parameters_[i].get();

        if (!parameter->arg()->hasDefaultValue() && !value.has_value())
        {
          std::string message;
          message = "Missing argument. Name: " + parameter->arg()->name + ". Index: " + std::to_string(parameter->arg()->position) + ".";
          throw std::invalid_argument(message);
        }
      }
    }

    return result;
  }

  inline Convertible Native::matchParameters(std::vector<std::optional<VALUE>>& values)
  {
    Convertible result = Convertible::Exact;
    for (size_t i = 0; i < this->parameters_.size(); i++)
    {
      ParameterAbstract* parameter = this->parameters_[i].get();
      std::optional<VALUE>& value = values[i];
      result = result & parameter->matches(value);
    }
    return result;
  }

  inline Resolved Native::matches(size_t argc, const VALUE* argv)
  {
    // Return false if Ruby provided more arguments than the C++ method takes
    if (argc > this->parameters_.size())
      return Resolved{ Convertible::None, 0, this };

    Resolved result{ Convertible::Exact, 1, this };

    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, false);
    result.convertible = this->matchParameters(rubyValues);

    if (this->parameters_.size() > 0)
    {
      size_t providedValues = std::count_if(rubyValues.begin(), rubyValues.end(), [](std::optional<VALUE>& value)
      {
        return value.has_value();
      });

      result.parameterMatch = providedValues / (double)this->parameters_.size();
    }
    return result;
  }

  inline std::vector<const ParameterAbstract*> Native::parameters()
  {
    std::vector<const ParameterAbstract*> result;

    std::transform(this->parameters_.begin(), this->parameters_.end(), std::back_inserter(result),
      [](std::unique_ptr<ParameterAbstract>& parameter) -> ParameterAbstract*
      {
        return parameter.get();
      });
      
    return result;
  }
RICE_DETAIL_END_NAMESPACE
