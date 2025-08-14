#include <stdexcept>

namespace Rice
{
  template<typename Enum_T>
  Enum<Enum_T>::Enum(char const* name, Module module) : Data_Type<Enum_T>()
  {
    Data_Type<Enum_T> klass = define_class_under<Enum_T>(module, name);
    define_methods(klass);
  }

  template<typename Enum_T>
  inline Enum<Enum_T>& Enum<Enum_T>::define_value(std::string name, Enum_T value)
  {
    // Save mapping from value to name
    valuesToNames_[value] = name;

    // Store value as class constant available to Ruby
    Data_Object<Enum_T> object(value, true, Enum<Enum_T>::klass());
    this->const_set(name, object);

    return *this;
  }

  template<typename Enum_T>
  inline void Enum<Enum_T>::define_methods(Data_Type<Enum_T> klass)
  {
    // First we need a constructor
    klass.define_constructor(Constructor<Enum_T>());

    klass.define_method("to_s", [](Enum_T& self) -> String
      {
        return String(valuesToNames_[self]);
      })
      .define_method("to_int", [](Enum_T& self) ->  Underlying_T
      {
        return (Underlying_T)(self);
      })
      .define_method("coerce", [](Enum_T& self, Underlying_T& other) -> Array
      {
        /* Other will be a numeric value that matches the underlying type of the enum, for example an int.
           Convert that to the enum type and then create new Ruby object to wrap it. This then enables code
           like this:
        
           Colors::Red | Colors:Blue | Colors:Green

        Colors::Red | Colors:Blue returns an integer. Then this method converts the integer back into an Enum
        instance so that Colors:Blue | Colors:Green works. */
        Enum_T otherEnum = (Enum_T)other;

        Array result;
        result.push(self, false);
        result.push(otherEnum, true);
        return result;
      })
      .define_method("inspect", [](Enum_T& self)
      {
        std::stringstream result;
        VALUE rubyKlass = Enum<Enum_T>::klass().value();
        result << "#<" << detail::protect(rb_class2name, rubyKlass)
          << "::" << Enum<Enum_T>::valuesToNames_[self] << ">";

        // We have to return string because we don't know if std::string support has
        // been included by the user
        return String(result.str());
      })
      .define_method("hash", [](Enum_T& self) ->  Underlying_T
      {
        return (Underlying_T)self;
      })
      .define_method("eql?", [](Enum_T& self, Enum_T& other)
      {
        return self == other;
    });

    // Add aliases
    rb_define_alias(klass, "===", "eql?");
    rb_define_alias(klass, "to_i", "to_int");

    // Add comparable support
    klass.include_module(rb_mComparable)
      .define_method("<=>", [](Enum_T& self, Enum_T& other)
      {
        if (self == other)
        {
          return 0;
        }
        else if (self < other)
        {
          return -1;
        }
        else
        {
          return 1;
        }
      });

    // Add ability to get enum values
    klass.define_singleton_method("values", [](VALUE ruby_klass) -> VALUE
    {
      Array result;

      for (auto& pair : valuesToNames_)
      {
        Object enumValue = Class(ruby_klass).const_get(pair.second);
        result.push(enumValue, false);
      }

      return result;
    }, Return().setValue());

    // Add bitwise operators
    klass.define_method("&", [](Enum_T& self, Enum_T& other) -> Underlying_T
      {
        return (Underlying_T)self & (Underlying_T)other;
      })
      .define_method("|", [](Enum_T& self, Enum_T& other) -> Underlying_T
      {
        return (Underlying_T)self | (Underlying_T)other;
      })
      .define_method("^", [](Enum_T& self, Enum_T& other) -> Underlying_T
      {
        return (Underlying_T)self ^ (Underlying_T)other;
      })
      .define_method("~", [](Enum_T& self) -> Underlying_T
      {
        return ~(Underlying_T)self;
      });

    // Add shift operators
    klass.define_method("<<", [](Enum_T& self, int shift) -> Underlying_T
      {
        return (Underlying_T)self << shift;
      })
      .define_method(">>", [](Enum_T& self, int shift) -> Underlying_T
      {
        return (Underlying_T)self >> shift;
      });

    // Add conversions from int
    klass.define_singleton_method("from_int", [](VALUE ruby_klass, int32_t value) -> Object
      {
        auto iter = Enum<Enum_T>::valuesToNames_.find((Enum_T)value);
        if (iter == Enum<Enum_T>::valuesToNames_.end())
        {
          throw std::runtime_error("Unknown enum value: " + std::to_string(value));
        }

        std::string name = iter->second;
        return Object(ruby_klass).const_get(name);
      });
  }

  template<typename Enum_T>
  Enum<Enum_T> define_enum(char const* name)
  {
    return define_enum_under<Enum_T>(name, rb_cObject);
  }

  template<typename Enum_T>
  Enum<Enum_T> define_enum_under(char const* name, Module module)
  {
    if (detail::Registries::instance.types.isDefined<Enum_T>())
    {
      return Enum<Enum_T>();
    }

    return Enum<Enum_T>(name, module);
  }
}