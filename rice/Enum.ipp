#include "detail/TypeRegistry.hpp"
#include "Data_Object.hpp"
#include "cpp_api/String.hpp"

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

    // Instance methods
    klass.define_method("to_s", [](Enum_T& self)
        {
          // We have to return string because we don't know if std::string support has
          // been included by the user
          return String(valuesToNames_[self]);
        })
      .define_method("to_i", [](Enum_T& self) ->  Underlying_T
        {
          return (Underlying_T)self;
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

    // Add comparable support
    klass.include_module(rb_mComparable);

        // Singleton methods
    klass.define_singleton_method("each", [](Class enum_class)
        {
//        Get rid of LocalJumpErrors
            if (rb_block_given_p())
            {
                for (auto& pair : valuesToNames_)
                {
                    Enum_T enumValue = pair.first;
                    VALUE value = detail::To_Ruby<Enum_T>().convert(enumValue);
                    detail::protect(rb_yield, value);
                }
                return Nil;
            } else {
                return enum_class.call("to_enum");
            }
      })
      .define_singleton_method("from_int", [](Class enum_class, int32_t value)
      {
          auto iter = Enum<Enum_T>::valuesToNames_.find((Enum_T)value);
          if (iter == Enum<Enum_T>::valuesToNames_.end())
          {
            throw std::runtime_error("Unknown enum value: " + std::to_string(value));
          }

          std::string name = iter->second;
          return enum_class.const_get(name);
      });
// same as klass.extend_object, which is not implemented
// since rb_extend_object(VALUE obj, VALUE module) is just rb_include_module(rb_singleton_class(obj), module);
    klass.singleton_class().include_module(rb_mEnumerable);
  }

  template<typename Enum_T>
  Enum<Enum_T> define_enum(char const* name, Module module)
  {
    if (detail::TypeRegistry::isDefined<Enum_T>())
    {
      return Enum<Enum_T>();
    }

    return Enum<Enum_T>(name, module);
  }
}
