#include "../traits/function_traits.hpp"
#include "../traits/rice_traits.hpp"
#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"
#include "../detail/RubyFunction.hpp"
#include "../Data_Type.hpp"

#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class UnorderedMapHelper
    {
      using Key_T = typename T::key_type;
      using Mapped_T = typename T::mapped_type;
      using Value_T = typename T::value_type;
      using Reference_T = typename T::reference;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;
      using To_Ruby_T = typename detail::remove_cv_recursive_t<Mapped_T>;

    public:
      UnorderedMapHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->register_pair();
        this->define_constructor();
        this->define_copyable_methods();
        this->define_capacity_methods();
        this->define_access_methods();
        this->define_comparable_methods();
        this->define_modify_methods();
        this->define_enumerable();
        this->define_to_s();
        this->define_to_hash();
      }

    private:

      void register_pair()
      {
        define_pair_auto<Value_T>();
      }

      void define_constructor()
      {
        klass_.define_constructor(Constructor<T>());
      }

      void define_copyable_methods()
      {
        if constexpr (std::is_copy_constructible_v<Value_T>)
        {
          klass_.define_method("copy", [](T& unordered_map) -> T
            {
              return unordered_map;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& unordered_map) -> T
            {
              throw std::runtime_error("Cannot copy unordered_maps with non-copy constructible types");
              return unordered_map;
            });
        }
      }

      void define_capacity_methods()
      {
        klass_.define_method("empty?", &T::empty)
          .define_method("max_size", &T::max_size)
          .define_method("size", &T::size);
        
        rb_define_alias(klass_, "count", "size");
        rb_define_alias(klass_, "length", "size");
      }

      void define_access_methods()
      {
        // Access methods
        klass_.define_method("[]", [](const T& unordered_map, const Key_T& key) -> std::optional<Mapped_T>
          {
            auto iter = unordered_map.find(key);

            if (iter != unordered_map.end())
            {
              return iter->second;
            }
            else
            {
              return std::nullopt;
            }
          })
          .define_method("include?", [](T& unordered_map, Key_T& key) -> bool
          {
              return unordered_map.find(key) != unordered_map.end();
          })
          .define_method("keys", [](T& unordered_map) -> std::vector<Key_T>
            {
              std::vector<Key_T> result;
              std::transform(unordered_map.begin(), unordered_map.end(), std::back_inserter(result),
                [](const auto& pair)
                {
                  return pair.first;
                });

              return result;
            })
          .define_method("values", [](T& unordered_map) -> std::vector<Mapped_T>
            {
              std::vector<Mapped_T> result;
              std::transform(unordered_map.begin(), unordered_map.end(), std::back_inserter(result),
                [](const auto& pair)
                {
                  return pair.second;
                });

              return result;
            });

            rb_define_alias(klass_, "has_key", "include?");
      }

      // Methods that require Value_T to support operator==
      void define_comparable_methods()
      {
        if constexpr (detail::is_comparable_v<Mapped_T>)
        {
          klass_.define_method("value?", [](T& unordered_map, Mapped_T& value) -> bool
            {
              auto it = std::find_if(unordered_map.begin(), unordered_map.end(),
              [&value](auto& pair)
                {
                  return pair.second == value;
                });

              return it != unordered_map.end();
          });
        }
        else
        {
          klass_.define_method("value?", [](T& unordered_map, Mapped_T& value) -> bool
          {
              return false;
          });
        }

        rb_define_alias(klass_, "has_value", "value?");
      }

      void define_modify_methods()
      {
        klass_.define_method("clear", &T::clear)
          .define_method("delete", [](T& unordered_map, Key_T& key) -> std::optional<Mapped_T>
            {
              auto iter = unordered_map.find(key);

              if (iter != unordered_map.end())
              {
                Mapped_T result = iter->second;
                unordered_map.erase(iter);
                return result;
              }
              else
              {
                return std::nullopt;
              }
            })
          .define_method("[]=", [](T& unordered_map, Key_T key, Mapped_T& value) -> Mapped_T
            {
              unordered_map[key] = value;
              return value;
            });

          rb_define_alias(klass_, "store", "[]=");
      }

      void define_enumerable()
      {
        // Add enumerable support
        klass_.template define_iterator<typename T::iterator (T::*)()>(&T::begin, &T::end);
      }

      void define_to_hash()
      {
        // Add enumerable support
        klass_.define_method("to_h", [](T& unordered_map)
        {
          VALUE result = rb_hash_new();
          std::for_each(unordered_map.begin(), unordered_map.end(), [&result](const Reference_T& pair)
          {
            VALUE key = detail::To_Ruby<Key_T&>().convert(pair.first);
            VALUE value = detail::To_Ruby<To_Ruby_T&>().convert(pair.second);
            rb_hash_aset(result, key, value);
          });

          return result;
        }, Return().setValue());
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<Key_T> && detail::is_ostreamable_v<Mapped_T>)
        {
          klass_.define_method("to_s", [](const T& unordered_map)
            {
              auto iter = unordered_map.begin();

              std::stringstream stream;
              stream << "{";

              for (; iter != unordered_map.end(); iter++)
              {
                if (iter != unordered_map.begin())
                {
                  stream << ", ";
                }
                stream << iter->first << " => " << iter->second;
              }

              stream << "}";
              return stream.str();
            });
        }
        else
        {
          klass_.define_method("to_s", [](const T& unordered_map)
            {
              return "[Not printable]";
            });
        }
      }

      private:
        Data_Type<T> klass_;
    };
  } // namespace

  template<typename T>
  Data_Type<T> define_unordered_map_under(Object parent, std::string name)
  {
    if (Data_Type<T>::check_defined(name, parent))
    {
      return Data_Type<T>();
    }

    Data_Type<T> result = define_class_under<detail::intrinsic_type<T>>(parent, name.c_str());
    stl::UnorderedMapHelper helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_unordered_map(std::string name)
  {
    if (Data_Type<T>::check_defined(name))
    {
      return Data_Type<T>();
    }

    Data_Type<T> result = define_class<detail::intrinsic_type<T>>(name.c_str());
    stl::UnorderedMapHelper<T> helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_unordered_map_auto()
  {
    Module rb_mStd = define_module("Std");
    std::string name = detail::typeName(typeid(T));
    std::string klassName = detail::makeClassName(name);
    return define_unordered_map_under<T>(rb_mStd, klassName);
  }
   
  namespace detail
  {
    template<typename T, typename U>
    struct Type<std::unordered_map<T, U>>
    {
      static bool verify()
      {
        Type<T>::verify();
        Type<U>::verify();

        if (!Data_Type<std::unordered_map<T, U>>::is_defined())
        {
          define_unordered_map_auto<std::unordered_map<T, U>>();
        }

        return true;
      }
    };

    template<typename T, typename U>
    struct UnorderedMapFromHash
    {
      static int convertPair(VALUE key, VALUE value, VALUE user_data)
      {
        std::unordered_map<T, U>* result = (std::unordered_map<T, U>*)(user_data);

        // This method is being called from Ruby so we cannot let any C++ 
        // exceptions propogate back to Ruby
        return cpp_protect([&]
        {
          result->operator[](From_Ruby<T>().convert(key)) = From_Ruby<U>().convert(value);
          return ST_CONTINUE;
        });
      }

      static std::unordered_map<T, U> convert(VALUE value)
      {
        std::unordered_map<T, U> result;
        VALUE user_data = (VALUE)(&result);

        // MSVC needs help here, but g++ does not
        using Rb_Hash_ForEach_T = void(*)(VALUE, int(*)(VALUE, VALUE, VALUE), VALUE);
        detail::protect<Rb_Hash_ForEach_T>(rb_hash_foreach, value, convertPair, user_data);

        return result;
      }
    };

    template<typename T, typename U>
    class From_Ruby<std::unordered_map<T, U>>
    {
    public:
      From_Ruby() = default;

      explicit From_Ruby(Arg * arg) : arg_(arg)
      {
      }

      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Convertible::Exact;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::unordered_map<T, U> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped unordered_map (hopefully!)
            return *detail::unwrap<std::unordered_map<T, U>>(value, Data_Type<std::unordered_map<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby hash and the unordered_mapped type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              return UnorderedMapFromHash<T, U>::convert(value);
            }
          }
          case RUBY_T_NIL:
          {
            if (this->arg_ && this->arg_->hasDefaultValue())
            {
              return this->arg_->template defaultValue<std::unordered_map<T, U>>();
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::unordered_map");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<typename T, typename U>
    class From_Ruby<std::unordered_map<T, U>&>
    {
    public:
      From_Ruby() = default;

      explicit From_Ruby(Arg * arg) : arg_(arg)
      {
      }

      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Convertible::Exact;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::unordered_map<T, U>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped unordered_map (hopefully!)
            return *detail::unwrap<std::unordered_map<T, U>>(value, Data_Type<std::unordered_map<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby array and the unordered_map type is copyable
            if constexpr (std::is_default_constructible_v<std::unordered_map<T, U>>)
            {
              this->converted_ = UnorderedMapFromHash<T, U>::convert(value);
              return this->converted_;
            }
          }
          case RUBY_T_NIL:
          {
            if (this->arg_ && this->arg_->hasDefaultValue())
            {
              return this->arg_->template defaultValue<std::unordered_map<T, U>>();
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::unordered_map");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
      std::unordered_map<T, U> converted_;
    };

    template<typename T, typename U>
    class From_Ruby<std::unordered_map<T, U>*>
    {
    public:
      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Convertible::Exact;
            break;
          case RUBY_T_NIL:
            return Convertible::Exact;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::unordered_map<T, U>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped unordered_map (hopefully!)
            return detail::unwrap<std::unordered_map<T, U>>(value, Data_Type<std::unordered_map<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby array and the unordered_map type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              this->converted_ = UnorderedMapFromHash<T, U>::convert(value);
              return &this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::unordered_map");
          }
        }
      }

    private:
      std::unordered_map<T, U> converted_;
    };
  }
}