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

    // Instance methods. The self parameter is confusing because it is really a Data_Object<Enum_T>.
    // However, if we make that the type then the From_Ruby code will consider it a 
    // Data_Type<Data_Object<Enum_T>>>. But in define class above it was actually bound as 
    // Data_Type<Enum_T>. Thus the static_casts in the methods below.
    klass.define_method("to_s", [](Enum_T& notSelf)
      {
        // We have to return string because we don't know if std::string support has
        // been included by the user
        Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
        return String(valuesToNames_[*self]);
      })
      .define_method("to_int", [](Enum_T& notSelf) ->  Underlying_T
      {
        Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
        return static_cast<Underlying_T>(*self);
      })
      .define_method("coerce", [](Enum_T& notSelf, Underlying_T& other) -> std::tuple<Enum_T, Data_Object<Enum_T>>
      {
        /* Other will be a numeric value that matches the underlying type of the enum, for example an int.
           Convert that to the enum type and then create new Ruby object to wrap it. This then enables code
           like this:
        
           Colors::Red | Colors:Blue | Colors:Green

        Colors::Red | Colors:Blue returns an integer. Then this method converts the integer back into an Enum
        instance so that Colors:Blue | Colors:Green works. */

        Enum_T enumValue = (Enum_T)other;
        Data_Object<Enum_T> object(enumValue, true, Enum<Enum_T>::klass());
        return std::tie<Enum_T, Data_Object<Enum_T>>(notSelf, object);
      })
      .define_method("inspect", [](Enum_T& notSelf)
      {
        Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);

        std::stringstream result;
        VALUE rubyKlass = Enum<Enum_T>::klass().value();
        result << "#<" << detail::protect(rb_class2name, rubyKlass)
          << "::" << Enum<Enum_T>::valuesToNames_[*self] << ">";

        // We have to return string because we don't know if std::string support has
        // been included by the user
        return String(result.str());
      })
      .define_method("hash", [](Enum_T& notSelf) ->  Underlying_T
      {
        Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
        return (Underlying_T)*self;
      })
      .define_method("eql?", [](Enum_T& notSelf, Enum_T& notOther)
      {
        Data_Object<Enum_T> self = static_cast<Data_Object<Enum_T>>(notSelf);
        Data_Object<Enum_T> other = static_cast<Data_Object<Enum_T>>(notOther);
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

    // Add enumerable support
    klass.include_module(rb_mEnumerable)
      .define_singleton_method("each", [](VALUE ruby_klass) -> VALUE
        {
          if (!rb_block_given_p())
          {
            return rb_enumeratorize_with_size(ruby_klass, Identifier("each").to_sym(),
              0, nullptr, 0);
          }

          for (auto& pair : valuesToNames_)
          {
            Enum_T enumValue = pair.first;
            VALUE value = detail::To_Ruby<Enum_T>().convert(enumValue);
            detail::protect(rb_yield, value);
          }

          return ruby_klass;
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