namespace Rice::detail
{
  inline bool Resolved::operator<(Resolved other)
  {
    return this->score < other.score;
  }

  inline bool Resolved::operator>(Resolved other)
  {
    return this->score > other.score;
  }

  inline VALUE Native::resolve(int argc, VALUE* argv, VALUE self)
  {
    /* This method is called from Ruby and is responsible for determining the correct
       Native object (ie, NativeFunction, NativeIterator, NativeAttributeGet and
       NativeAttributeSet) that should be used to invoke the underlying C++ code.
       Most of the time there will be a single Native object registered for a C++ function,
       method, constructor, iterator or attribute. However, there can be multiple Natives
       when a C++ function/method/constructor is overloaded.

       In that case, the code iterates over each Native and calls its matches method. The matches
       method returns a Resolved object with a numeric score (0.0 to 1.0). The score is computed as:

         score = minParameterScore * parameterMatch

       where minParameterScore is the minimum score across all passed parameters (using precision-based
       scoring for numeric types), and parameterMatch applies a small penalty (0.99) for each default
       parameter used. If not enough arguments are provided and missing parameters don't have defaults,
       the method returns 0 (not viable).

       The method sorts the Natives and picks the one with the highest score. Given these two C++ functions:

       void some_method(int a);
       void some_method(int a, float b = 2.0);

       A call from ruby of some_method(1) will match both signatures, but the first one
       will be chosen because parameterMatch = 1.0 for the first overload but 0.99 for the second. */

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
      std::map<std::string, VALUE> values = readRubyArgs(argc, argv);

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
            return native->matches(values);
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
        if (resolved.score > Convertible::None)
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
      return (*native)(values, self);
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
      std::string argName = "arg_" + std::to_string(i);
      argsVector.emplace_back(std::make_unique<Arg>(argName));
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

  inline std::map<std::string, VALUE> Native::readRubyArgs(size_t argc, const VALUE* argv)
  {
    std::map<std::string, VALUE> result;

    // Keyword handling
    if (rb_keyword_given_p())
    {
      // Keywords are stored in the last element in a hash
      size_t actualArgc = argc - 1;

      // Copy over leading non-keyword arguments
      for (size_t i = 0; i < actualArgc; i++)
      {
        std::string key = "arg_" + std::to_string(i);
        result[key] = argv[i];
      }

      VALUE value = argv[actualArgc];
      Hash keywords(value);

      // Copy over keyword arguments
      for (auto pair : keywords)
      {
        result[pair.first.to_s().str()] = pair.second.value();
      }
    }
    else
    {
      // Copy over leading non-keyword arguments
      for (size_t i = 0; i < argc; i++)
      {
        std::string key = "arg_" + std::to_string(i);
        result[key] = argv[i];
      }
    }

    // Block handling. If we find a block and the last parameter is missing then
    // set it to the block
    if (rb_block_given_p())// FIXME && result.size() > 0 && !result.back().second.has_value())
    {
      VALUE proc = rb_block_proc();
      std::string key = "arg_" + std::to_string(result.size());
      result[key] = proc;
    }

    return result;
  }

  inline std::vector<std::optional<VALUE>> Native::getRubyValues(std::map<std::string, VALUE> values, bool validate)
  {
    // !!!NOTE!!! We copied the values parameter because we are going to modify it!

    // Protect against user sending too many arguments
    if (values.size() > this->parameters_.size())
    {
      std::string message = "wrong number of arguments (given " +
        std::to_string(values.size()) + ", expected " + std::to_string(this->parameters_.size()) + ")";
      throw std::invalid_argument(message);
    }

    std::vector<std::optional<VALUE>> result(this->parameters_.size());

    for (size_t i=0; i< this->parameters_.size(); i++)
    {
      std::unique_ptr<ParameterAbstract>& parameter = this->parameters_[i];
      Arg* arg = parameter->arg();

      // If using keywords arguments, then the value key will be arg->name(). If using positional 
      // arguments then they key will be "arg_<position>"
      std::string keywordKey = arg->name;
      std::string positionKey = "arg_" + std::to_string(i);

      auto iter = values.find(keywordKey);
      if (iter == values.end() && keywordKey != positionKey)
      {
        iter = values.find(positionKey);
      }

      if (iter != values.end())
      {
        result[i] = iter->second;
        // Remove the value
        values.erase(iter);
      }
      else if (arg->hasDefaultValue())
      {
        result[i] = parameter->defaultValueRuby();
      }
      else if (validate)
      {
        std::string message = "Missing argument. Name: " + arg->name + ". Index: " + std::to_string(i) + ".";
        throw std::invalid_argument(message);
      }
    }

    // Check for unknown arguments
    if (validate && values.size() > 0)
    {
      // There are unknown arguments
      std::ostringstream message;
      message << "Unknown argument(s): ";
      size_t count = 0;
      for (const std::pair<const std::string, VALUE>& pair : values)
      {
        if (count > 0)
          message << ", ";
        message << pair.first;
        count++;
      }
      throw std::invalid_argument(message.str());
    }

    return result;
  }

  inline double Native::matchParameters(std::vector<std::optional<VALUE>>& values, size_t argc)
  {
    // Only score arguments actually passed (not defaults)
    double minScore = Convertible::Exact;

    for (size_t i = 0; i < argc && i < this->parameters_.size(); i++)
    {
      ParameterAbstract* parameter = this->parameters_[i].get();
      std::optional<VALUE>& value = values[i];
      double score = parameter->matches(value);
      minScore = (std::min)(minScore, score);
    }

    return minScore;
  }

  inline Resolved Native::matches(std::map<std::string, VALUE>& values)
  {
    // Return Convertible::None if Ruby provided more arguments than the C++ method takes
    if (values.size() > this->parameters_.size())
    {
      return Resolved{ Convertible::None, this };
    }

    // Get Ruby values for each parameter and see how they match
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(values, false);
    double minScore = this->matchParameters(rubyValues, values.size());

    // If zero score return then stop
    if (minScore == 0)
    {
      return Resolved{ Convertible::None, this };
    }

    // How many actual values do we have?
    size_t actualValuesCount = std::count_if(rubyValues.begin(), rubyValues.end(),
      [](std::optional<VALUE>& optional)
      {
        return optional.has_value();
      });

    // If we don't have enough parameters return
    if (actualValuesCount < this->parameters_.size())
      return Resolved{ Convertible::None, this };

    // Penalize use of default parameters
    double parameterMatch = Convertible::Exact;
    size_t defaultParameterCount = actualValuesCount - values.size();
    for (size_t i = 0; i < defaultParameterCount; i++)
    {
      parameterMatch *= 0.99;  // Small penalty per default used
    }

    // Final score: minScore * parameterMatch
    double finalScore = minScore * parameterMatch;

    return Resolved{ finalScore, this };
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
}
