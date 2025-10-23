
namespace Rice::detail
{
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
    return cpp_protect([&]
    {
      Identifier id(methodId);
      std::string methodName = id.str();
      std::string className = rb_class2name(klass);

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
            return native->matches(argc, argv, self);
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

  inline Native::Native(std::vector<std::unique_ptr<ParameterAbstract>>&& parameters) : parameters_(std::move(parameters))
  {
  }

  inline ParameterAbstract* Native::getParameterByName(std::string name)
  {
    for (std::unique_ptr<ParameterAbstract>& parameter : this->parameters_)
    {
      if (parameter->arg->name == name)
      {
        return parameter.get();
      }
    }

    return nullptr;
  }

  // -----------  Helpers ----------------
  template<typename T>
  inline void Native::verify_type(bool isBuffer)
  {
    using Base_T = std::remove_pointer_t<remove_cv_recursive_t<T>>;

    detail::verifyType<T>();

    if constexpr (std::is_pointer_v<T> && std::is_fundamental_v<std::remove_pointer_t<T>>)
    {
      Type<Pointer<Base_T>>::verify();
      Type<Buffer<Base_T>>::verify();
    }
    else if constexpr (std::is_array_v<T>)
    {
      Type<Pointer<std::remove_extent_t<remove_cv_recursive_t<T>>>>::verify();
      Type<Buffer<std::remove_extent_t<remove_cv_recursive_t<T>>>>::verify();
    }
    else if (isBuffer)
    {
      if constexpr (std::is_pointer_v<T> && !std::is_function_v<Base_T> && !std::is_abstract_v<Base_T>)
      {
        Type<Pointer<Base_T>>::verify();
        Type<Buffer<Base_T>>::verify();
      }
      else
      {
        static_assert(true, "Only pointer types can be marked as buffers");
      }
    }
  }

  template<typename Tuple_T, std::size_t ...Indices>
  inline void Native::verify_args(MethodInfo* methodInfo, std::index_sequence<Indices...> indices)
  {
    (Native::verify_type<std::tuple_element_t<Indices, Tuple_T>>(methodInfo->arg(Indices)->isBuffer()), ...);
  }

  template<typename Tuple_T, std::size_t ...Indices>
  inline void Native::create_parameters_impl(std::vector<std::unique_ptr<ParameterAbstract>>& parameters, MethodInfo* methodInfo, std::index_sequence<Indices...> indices)
  {
    (parameters.push_back(std::move(std::make_unique<Parameter<std::tuple_element_t<Indices, Tuple_T>>>(methodInfo->arg(Indices)))), ...);
  }

  template<typename Tuple_T>
  inline std::vector<std::unique_ptr<ParameterAbstract>> Native::create_parameters(MethodInfo* methodInfo)
  {
    std::vector<std::unique_ptr<ParameterAbstract>> result;
    auto indices = std::make_index_sequence<std::tuple_size_v<Tuple_T>>{};
    Native::create_parameters_impl<Tuple_T>(result, methodInfo, indices);
    return result;
  }

  inline std::vector<std::optional<VALUE>> Native::getRubyValues(size_t argc, const VALUE* argv, bool validate)
  {
#undef max
    int size = std::max(this->parameters_.size(), argc);
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
        ParameterAbstract* parameter = this->getParameterByName(key.str());
        if (!parameter)
        {
          throw std::invalid_argument("Unknown keyword: " + key.str());
        }

        const Arg* arg = parameter->arg;

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

        if (!parameter->arg->hasDefaultValue() && !value.has_value())
        {
          std::string message;
          message = "Missing argument. Name: " + parameter->arg->name + ". Index: " + std::to_string(parameter->arg->position) + ".";
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

  inline Resolved Native::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    // Return false if Ruby provided more arguments than the C++ method takes
    if (argc > this->parameters_.size())
      return Resolved{ Convertible::None, 0, this };

    Resolved result{ Convertible::Exact, 1, this };

    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, false);
    result.convertible = this->matchParameters(rubyValues);

    if (this->parameters_.size() > 0)
    {
      int providedValues = std::count_if(rubyValues.begin(), rubyValues.end(), [](std::optional<VALUE>& value)
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
}
