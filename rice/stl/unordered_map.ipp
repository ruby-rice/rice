#include "../detail/rice_traits.hpp"
#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"
#include "../detail/RubyFunction.hpp"
#include "../Data_Type.hpp"

#include <sstream>
#include <stdexcept>
#include <unordered_map>

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
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;

    public:
      UnorderedMapHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->define_constructor();
        this->define_copyable_methods();
        this->define_capacity_methods();
        this->define_access_methods();
        this->define_comparable_methods();
        this->define_modify_methods();
        this->define_to_s();
        this->define_enumerable();
      }

    private:

      void define_constructor()
      {
        klass_.define_constructor(Constructor<T>());
      }

      void define_copyable_methods()
      {
        if constexpr (std::is_copy_constructible_v<Value_T>)
        {
          klass_.define_method("copy", [](T& self) -> T
            {
              return self;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& self) -> T
            {
              throw std::runtime_error("Cannot copy unordered_maps with non-copy constructible types");
              return self;
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
        klass_.define_method("[]", [](const T& self, const Key_T& key) -> std::optional<Mapped_T>
          {
            auto iter = self.find(key);

            if (iter != self.end())
            {
              return iter->second;
            }
            else
            {
              return std::nullopt;
            }
          })
          .define_method("include?", [](T& self, Key_T& key) -> bool
          {
              return self.find(key) != self.end();
          })
          .define_method("keys", [](T& self) -> std::vector<Key_T>
            {
              std::vector<Key_T> result;
              std::transform(self.begin(), self.end(), std::back_inserter(result),
                [](const auto& pair)
                {
                  return pair.first;
                });

              return result;
            })
          .define_method("values", [](T& self) -> std::vector<Mapped_T>
            {
              std::vector<Mapped_T> result;
              std::transform(self.begin(), self.end(), std::back_inserter(result),
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
          klass_.define_method("value?", [](T& self, Mapped_T& value) -> bool
            {
              auto it = std::find_if(self.begin(), self.end(),
              [&value](auto& pair)
                {
                  return pair.second == value;
                });

              return it != self.end();
          });
        }
        else
        {
          klass_.define_method("value?", [](T& self, Mapped_T& value) -> bool
          {
              return false;
          });
        }

        rb_define_alias(klass_, "has_value", "value?");
      }

      void define_modify_methods()
      {
        klass_.define_method("clear", &T::clear)
          .define_method("delete", [](T& self, Key_T& key) -> std::optional<Mapped_T>
            {
              auto iter = self.find(key);

              if (iter != self.end())
              {
                Mapped_T result = iter->second;
                self.erase(iter);
                return result;
              }
              else
              {
                return std::nullopt;
              }
            })
          .define_method("[]=", [](T& self, Key_T key, Mapped_T value) -> Mapped_T
            {
              self[key] = value;
              return value;
            });

          rb_define_alias(klass_, "store", "[]=");
      }

      void define_enumerable()
      {
        // Add enumerable support
        klass_.include_module(rb_mEnumerable)
          .define_method("each", [](T& self) -> const T&
            {
              for (Value_T& pair : self)
              {
                VALUE key = detail::To_Ruby<Key_T>().convert(pair.first);
                VALUE value = detail::To_Ruby<Mapped_T>().convert(pair.second);
                VALUE result = rb_ary_new3(2, key, value);
                rb_yield(result);
              }
              return self;
            });
      }

      void define_to_hash()
      {
        // Add enumerable support
        klass_.define_method("to_h", [](T& self)
        {
          VALUE result = rb_hash_new();
          std::for_each(self.begin(), self.end(), [&result](const auto& pair)
          {
            VALUE key = detail::To_Ruby<Key_T>().convert(pair->first);
            VALUE value = detail::To_Ruby<Mapped_T>().convert(pair->second);
            rb_hash_aset(result, key, value);
          });

          return result;
        });
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<Key_T> && detail::is_ostreamable_v<Mapped_T>)
        {
          klass_.define_method("to_s", [](const T& self)
            {
              auto iter = self.begin();

              std::stringstream stream;
              stream << "{";

              for (; iter != self.end(); iter++)
              {
                if (iter != self.begin())
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
          klass_.define_method("to_s", [](const T& self)
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
  Data_Type<T> define_unordered_map_under(Object module, std::string name)
  {
    if (detail::TypeRegistry::isDefined<T>())
    {
      // If the unordered_map has been previously seen it will be registered but may
      // not be associated with the constant Module::<name>
      module.const_set_maybe(name, Data_Type<T>().klass());
      return Data_Type<T>();
    }

    Data_Type<T> result = define_class_under<detail::intrinsic_type<T>>(module, name.c_str());
    stl::UnorderedMapHelper helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_unordered_map(std::string name)
  {
    if (detail::TypeRegistry::isDefined<T>())
    {
      // If the unordered_map has been previously seen it will be registered but may
      // not be associated with the constant Object::<name>
      Object(rb_cObject).const_set_maybe(name, Data_Type<T>().klass());
      return Data_Type<T>();
    }

    Data_Type<T> result = define_class<detail::intrinsic_type<T>>(name.c_str());
    stl::UnorderedMapHelper<T> helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_unordered_map_auto()
  {
    std::string klassName = detail::makeClassName(typeid(T));
    Module rb_mRice = define_module("Rice");
    Module rb_munordered_map = define_module_under(rb_mRice, "Std");
    return define_unordered_map_under<T>(rb_munordered_map, klassName);
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

        if (!detail::TypeRegistry::isDefined<std::unordered_map<T, U>>())
        {
          define_unordered_map_auto<std::unordered_map<T, U>>();
        }

        return true;
      }
    };

    template<typename T, typename U>
    int unordered_mapFromHashPair(VALUE key, VALUE value, VALUE user_data)
    {
      std::unordered_map<T, U> *unordered_map = (std::unordered_map<T, U>*)(user_data);

      // This method is being called from Ruby so we cannot let any C++ 
      // exceptions propogate back to Ruby
      return cpp_protect([&]
        {
          unordered_map->operator[](From_Ruby<T>().convert(key)) = From_Ruby<U>().convert(value);
          return ST_CONTINUE;
        });
    }

    template<typename T, typename U>
    std::unordered_map<T, U> unordered_mapFromHash(VALUE value)
    {
      std::unordered_map<T, U> result;
      VALUE user_data = (VALUE)(&result);
      // Should use protect but older compilers won't compile this (latest MSVC and GCC are fine)
      //detail::protect<void>(rb_hash_foreach, value, mapFromHashPair<T, U>, user_data);
      rb_hash_foreach(value, unordered_mapFromHashPair<T, U>, user_data);

      return result;
    }

    template<typename T, typename U>
    class From_Ruby<std::unordered_map<T, U>>
    {
    public:
      From_Ruby() = default;

      explicit From_Ruby(Arg * arg) : arg_(arg)
      {
      }

      std::unordered_map<T, U> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case T_DATA:
          {
            // This is a wrapped unordered_map (hopefully!)
            return *Data_Object<std::unordered_map<T, U>>::from_ruby(value);
          }
          case T_HASH:
          {
            // If this an Ruby hash and the unordered_mapped type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              return unordered_mapFromHash<T, U>(value);
            }
          }
          case T_NIL:
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

      std::unordered_map<T, U>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case T_DATA:
          {
            // This is a wrapped unordered_map (hopefully!)
            return *Data_Object<std::unordered_map<T, U>>::from_ruby(value);
          }
          case T_HASH:
          {
            // If this an Ruby array and the unordered_map type is copyable
            if constexpr (std::is_default_constructible_v<std::unordered_map<T, U>>)
            {
              this->converted_ = unordered_mapFromHash<T, U>(value);
              return this->converted_;
            }
          }
          case T_NIL:
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
      std::unordered_map<T, U>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case T_DATA:
          {
            // This is a wrapped unordered_map (hopefully!)
            return Data_Object<std::unordered_map<T, U>>::from_ruby(value);
          }
          case T_HASH:
          {
            // If this an Ruby array and the unordered_map type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              this->converted_ = unordered_mapFromHash<T, U>(value);
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