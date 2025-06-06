
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
        rb_raise(rb_eArgError, "Could not find method call for %s#%s", rb_class2name(klass), identifier.c_str());
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

            rb_raise(rb_eArgError, message.str().c_str(), rb_class2name(klass), identifier.c_str(), natives.size());
          }
        }
      }

      // Call the C++ function
      return (*native)(argc, argv, self);
    });
  }
}
