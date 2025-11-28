#include <map>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class MultimapHelper
    {
      using Key_T = typename T::key_type;
      using Mapped_T = typename T::mapped_type;
      using Value_T = typename T::value_type;
      using Reference_T = typename T::reference;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;
      using To_Ruby_T = typename detail::remove_cv_recursive_t<Mapped_T>;

    public:
      MultimapHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->register_pair();
        this->define_constructors();
        this->define_capacity_methods();
        this->define_access_methods();
        this->define_comparable_methods();
        this->define_modify_methods();
        this->define_enumerable();
        this->define_to_s();
      }

    private:

      void register_pair()
      {
        define_pair<const Key_T, Mapped_T>();
      }

      void define_constructors()
      {
        klass_.define_constructor(Constructor<T>());

        if constexpr (std::is_copy_constructible_v<Key_T> && std::is_copy_constructible_v<Value_T>)
        {
          klass_.define_constructor(Constructor<T, const T&>());
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
        klass_.
          define_method("[]", [](T& multimap, const Key_T& key) -> Array
          {
            Array result;
            auto range = multimap.equal_range(key);

            for (auto iter = range.first; iter != range.second; iter++)
            {
              result.push(iter->second, false);
            }

            return result;
          })
          .define_method("include?", [](T& multimap, Key_T& key) -> bool
          {
              return multimap.find(key) != multimap.end();
          })
          .define_method("keys", [](T& multimap) -> std::vector<Key_T>
            {
              std::vector<Key_T> result;
              std::transform(multimap.begin(), multimap.end(), std::back_inserter(result),
                [](const auto& pair)
                {
                  return pair.first;
                });

              return result;
            })
          .define_method("values", [](T& multimap) -> std::vector<Mapped_T>
            {
              std::vector<Mapped_T> result;
              std::transform(multimap.begin(), multimap.end(), std::back_inserter(result),
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
          klass_.define_method("==", [](T& multimap, T& other)->bool
          {
            return multimap == other;
          })
          .define_method("value?", [](T& multimap, Mapped_T& value) -> bool
          {
            auto it = std::find_if(multimap.begin(), multimap.end(),
            [&value](auto& pair)
              {
                return pair.second == value;
              });

            return it != multimap.end();
          });
          rb_define_alias(klass_, "eql?", "==");
        }
        else
        {
          klass_.define_method("value?", [](T&, Mapped_T&) -> bool
          {
              return false;
          });
        }

        rb_define_alias(klass_, "has_value", "value?");
      }

      void define_modify_methods()
      {
        klass_.define_method("clear", &T::clear)
          .define_method("delete", [](T& multimap, Key_T& key) -> std::optional<Mapped_T>
          {
            auto iter = multimap.find(key);

            if (iter != multimap.end())
            {
              Mapped_T result = iter->second;
              multimap.erase(iter);
              return result;
            }
            else
            {
              return std::nullopt;
            }
          })
          .define_method("insert", [](T& map, Key_T key, Mapped_T& value) -> Mapped_T
          {
            Value_T element{ key, value };
            map.insert(element);
            return value;
          });
      }

      void define_enumerable()
      {
        // Add enumerable support
        klass_.template define_iterator<typename T::iterator (T::*)()>(&T::begin, &T::end);
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<Key_T> && detail::is_ostreamable_v<Mapped_T>)
        {
          klass_.define_method("to_s", [](const T& multimap)
            {
              auto iter = multimap.begin();

              std::stringstream stream;
              detail::TypeMapper<T> typeMapper;
              stream << "<" << typeMapper.rubyName() << ":";
              stream << "{";

              for (; iter != multimap.end(); iter++)
              {
                if (iter != multimap.begin())
                {
                  stream << ", ";
                }
                stream << iter->first << " => " << iter->second;
              }

              stream << "}>";
              return stream.str();
            });
        }
        else
        {
          klass_.define_method("to_s", [](const T&)
          {
            return "[Not printable]";
          });
        }
      }

      private:
        Data_Type<T> klass_;
    };
  } // namespace

  template<typename Key, typename T>
  Data_Type<std::multimap<Key, T>> define_multimap(std::string klassName)
  {
    using MultiMap_T = std::multimap<Key, T>;
    using Data_Type_T = Data_Type<MultiMap_T>;

    if (klassName.empty())
    {
      detail::TypeMapper<MultiMap_T> typeMapper;
      klassName = typeMapper.rubyName();
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<MultiMap_T>>(rb_mStd, id);
    stl::MultimapHelper helper(result);
    return result;
  }

  namespace detail
  {
    // Helper method - maybe someday create a C++ Ruby set wrapper
    template<typename T, typename U>
    std::multimap<T, U> toMultimap(VALUE rubyHash)
    {
      using Function_T = void(*)(VALUE, int(*)(VALUE, VALUE, VALUE), VALUE);

      auto block = [](VALUE key, VALUE value, VALUE user_data) -> int
      {
        using Key_T = typename std::multimap<T, U>::key_type;
        using Mapped_T = typename std::multimap<T, U>::mapped_type;
        using Value_T = typename std::multimap<T, U>::value_type;

        return cpp_protect([&]
        {
          Value_T pair = { From_Ruby<Key_T>().convert(key), From_Ruby<Mapped_T>().convert(value) };
          std::multimap<T, U>* result = (std::multimap<T, U>*)user_data;
          result->insert(pair);
          return ST_CONTINUE;
        });
      };

      std::multimap<T, U> result;
      detail::protect<Function_T>(rb_hash_foreach, rubyHash, block, (VALUE)&result);
      return result;
    }

    template<typename Key_T, typename T>
    struct Type<std::multimap<Key_T, T>>
    {
      static bool verify()
      {
        Type<Key_T>::verify();
        Type<Key_T>::verify();

        if (!Data_Type<std::multimap<Key_T, T>>::is_defined())
        {
          define_multimap<Key_T, T>();
        }

        return true;
      }
    };

    template<typename T, typename U>
    class From_Ruby<std::multimap<T, U>>
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
            return Data_Type<std::multimap<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::multimap<T, U> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped multimap (hopefully!)
            return *detail::unwrap<std::multimap<T, U>>(value, Data_Type<std::multimap<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby hash and the multimapped type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              return toMultimap<T, U>(value);
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::multimap");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<typename T, typename U>
    class From_Ruby<std::multimap<T, U>&>
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
            return Data_Type<std::multimap<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::multimap<T, U>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped multimap (hopefully!)
            return *detail::unwrap<std::multimap<T, U>>(value, Data_Type<std::multimap<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby array and the multimap type is copyable
            if constexpr (std::is_default_constructible_v<std::multimap<T, U>>)
            {
              this->converted_ = toMultimap<T, U>(value);
              return this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::multimap");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
      std::multimap<T, U> converted_;
    };

    template<typename T, typename U>
    class From_Ruby<std::multimap<T, U>*>
    {
    public:
      From_Ruby() = default;

      explicit From_Ruby(Arg* arg) : arg_(arg)
      {
      }

      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Data_Type<std::multimap<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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

      std::multimap<T, U>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped multimap (hopefully!)
            return detail::unwrap<std::multimap<T, U>>(value, Data_Type<std::multimap<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby array and the multimap type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              this->converted_ = toMultimap<T, U>(value);
              return &this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::multimap");
          }
        }
      }

    private:
      Arg* arg_;
      std::multimap<T, U> converted_;
    };
  }
}