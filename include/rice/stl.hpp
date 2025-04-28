#ifndef Rice__stl__hpp_
#define Rice__stl__hpp_


// =========   exception.hpp   =========

namespace Rice::stl
{
  extern Class rb_cStlException;
}


// ---------   exception.ipp   ---------
#include <exception>

// Libraries sometime inherit custom exception objects from std::exception,
// so define it for Ruby if necessary
namespace Rice::stl
{
  inline Class rb_cStlException;

  inline void define_stl_exception()
  {
    Module rb_mStd = define_module("Std");
    rb_cStlException = define_class_under<std::exception>(rb_mStd, "Exception", rb_eStandardError).
                        define_constructor(Constructor<std::exception>()).
                        define_method("message", &std::exception::what);
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::exception>
  {
    static bool verify()
    {
      Rice::stl::define_stl_exception();
      return true;
    }
  };
}


// =========   exception_ptr.hpp   =========


// ---------   exception_ptr.ipp   ---------
#include <exception>

namespace Rice::stl
{
  inline Data_Type<std::exception_ptr> define_exception_ptr()
  {
    Module rb_mStd = define_module("Std");
    return define_class_under<std::exception_ptr>(rb_mStd, "ExceptionPtr");
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::exception_ptr>
  {
    static bool verify()
    {
      if (!Data_Type<std::exception_ptr>::is_defined())
      {
        stl::define_exception_ptr();
      }

      return true;
    }
  };
}


// =========   string.hpp   =========


// ---------   string.ipp   ---------
#include <string>

namespace Rice::detail
{
  template<>
  struct Type<std::string>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<std::string>
  {
  public:
    VALUE convert(const std::string& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class To_Ruby<std::string&>
  {
  public:
    VALUE convert(const std::string& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class To_Ruby<std::string*>
  {
  public:
    VALUE convert(const std::string* x)
    {
      return detail::protect(rb_external_str_new, x->data(), (long)x->size());
    }
  };

  template<>
  class To_Ruby<std::string*&>
  {
  public:
    VALUE convert(const std::string* x)
    {
      return detail::protect(rb_external_str_new, x->data(), (long)x->size());
    }
  };

  template<>
  class To_Ruby<std::string**>
  {
  public:
    VALUE convert(std::string** data)
    {
      Buffer<std::string*> buffer(data);
      Data_Object<Buffer<std::string*>> dataObject(std::move(buffer));
      return dataObject.value();
    }
  };

  template<>
  class From_Ruby<std::string>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      return std::string(RSTRING_PTR(value), RSTRING_LEN(value));
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<std::string&>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string& convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return this->converted_;
    }

  private:
    Arg* arg_ = nullptr;
    std::string converted_ = "";
  };

  template<>
  class From_Ruby<std::string*>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string* convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return &this->converted_;
    }

  private:
    std::string converted_;
  };
}

// =========   string_view.hpp   =========


// ---------   string_view.ipp   ---------
#include <string_view>

namespace Rice::detail
{
  template<>
  struct Type<std::string_view>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<std::string_view>
  {
  public:
    VALUE convert(std::string_view const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class To_Ruby<std::string_view&>
  {
  public:
    VALUE convert(std::string_view const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class From_Ruby<std::string_view>
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
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::string_view convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      return std::string_view(RSTRING_PTR(value), RSTRING_LEN(value));
    }

  private:
    Arg* arg_ = nullptr;
  };
}

// =========   complex.hpp   =========


// ---------   complex.ipp   ---------
#include <complex>

namespace Rice::detail
{
  template<typename T>
  struct Type<std::complex<T>>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<typename T>
  class To_Ruby<std::complex<T>>
  {
  public:
    VALUE convert(const std::complex<T>& data)
    {
      std::vector<VALUE> args(2);
      args[0] = To_Ruby<T>().convert(data.real());
      args[1] = To_Ruby<T>().convert(data.imag());
      return protect(rb_funcallv, rb_mKernel, rb_intern("Complex"), (int)args.size(), (const VALUE*)args.data());
    }
  };

  template<typename T>
  class To_Ruby<std::complex<T>&>
  {
  public:
    VALUE convert(const std::complex<T>& data)
    {
      std::vector<VALUE> args(2);
      args[0] = To_Ruby<T>().convert(data.real());
      args[1] = To_Ruby<T>().convert(data.imag());
      return protect(rb_funcallv, rb_mKernel, rb_intern("Complex"), (int)args.size(), (const VALUE*)args.data());
    }
  };

  template<typename T>
  class From_Ruby<std::complex<T>>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_COMPLEX:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::complex<T> convert(VALUE value)
    {
      VALUE real = protect(rb_funcallv, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcallv, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);

      return std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));
    }
  };

  template<typename T>
  class From_Ruby<std::complex<T>&>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_COMPLEX:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::complex<T>& convert(VALUE value)
    {
      VALUE real = protect(rb_funcallv, value, rb_intern("real"), 0, (const VALUE*)nullptr);
      VALUE imaginary = protect(rb_funcallv, value, rb_intern("imaginary"), 0, (const VALUE*)nullptr);
      this->converted_ = std::complex<T>(From_Ruby<T>().convert(real), From_Ruby<T>().convert(imaginary));

      return this->converted_;
    }

  private:
    std::complex<T> converted_;
  };
}


// =========   optional.hpp   =========


// ---------   optional.ipp   ---------
#include <optional>

namespace Rice::detail
{
  template<typename T>
  struct Type<std::optional<T>>
  {
    constexpr static bool verify()
    {
      return Type<intrinsic_type<T>>::verify();
    }
  };

  template<>
  class To_Ruby<std::nullopt_t>
  {
  public:
    VALUE convert(const std::nullopt_t& _)
    {
      return Qnil;
    }
  };

  template<typename T>
  class To_Ruby<std::optional<T>>
  {
  public:
    static VALUE convert(const std::optional<T>& data, bool takeOwnership = false)
    {
      if (data.has_value())
      {
        return To_Ruby<T>().convert(data.value());
      }
      else
      {
        return Qnil;
      }
    }
  };

  template<typename T>
  class To_Ruby<std::optional<T>&>
  {
  public:
    static VALUE convert(const std::optional<T>& data, bool takeOwnership = false)
    {
      if (data.has_value())
      {
        return To_Ruby<T>().convert(data.value());
      }
      else
      {
        return Qnil;
      }
    }
  };

  template<typename T>
  class From_Ruby<std::optional<T>>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_NIL:
          return Convertible::Exact;
          break;
        default:
          return From_Ruby<T>().is_convertible(value);
      }
    }

    std::optional<T> convert(VALUE value)
    {
      if (value == Qnil)
      {
        return std::nullopt;
      }
      else
      {
        return From_Ruby<T>().convert(value);
      }
    }
  };

  template<typename T>
  class From_Ruby<std::optional<T>&>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_NIL:
          return Convertible::Exact;
          break;
        default:
          return From_Ruby<T>().is_convertible(value);
      }
    }

    std::optional<T>& convert(VALUE value)
    {
      if (value == Qnil)
      {
        this->converted_ = std::nullopt;
      }
      else
      {
        this->converted_ = From_Ruby<T>().convert(value);
      }
      return this->converted_;
    }
  private:
    std::optional<T> converted_;
  };
}


// =========   reference_wrapper.hpp   =========


// ---------   reference_wrapper.ipp   ---------
#include <functional>

namespace Rice::detail
{
  template<typename T>
  struct Type<std::reference_wrapper<T>>
  {
    constexpr static bool verify()
    {
      return Type<T>::verify();
    }
  };

  template<typename T>
  class To_Ruby<std::reference_wrapper<T>>
  {
  public:
    VALUE convert(const std::reference_wrapper<T>& data, bool takeOwnership = false)
    {
      return To_Ruby<T&>().convert(data.get());
    }
  };

  template<typename T>
  class From_Ruby<std::reference_wrapper<T>>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return this->converter_.is_convertible(value);
    }

    std::reference_wrapper<T> convert(VALUE value)
    {
      return this->converter_.convert(value);
    }

  private:
    From_Ruby<T&> converter_;
  };
}


// =========   pair.hpp   =========

namespace Rice
{
  template<typename T1, typename T2>
  Data_Type<std::pair<T1, T2>> define_pair(std::string klassName = "");
}


// ---------   pair.ipp   ---------
#include <utility>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class PairHelper
    {
    public:
      PairHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->define_constructor();
        this->define_copyable_methods();
        this->define_access_methods();
        this->define_modify_methods();
        this->define_to_s();
      }

    private:
      void define_constructor()
      {
        klass_.define_constructor(Constructor<T, typename T::first_type&, typename T::second_type&>());
      }

      void define_copyable_methods()
      {
        if constexpr (std::is_copy_constructible_v<typename T::first_type> && std::is_copy_constructible_v<typename T::second_type>)
        {
          klass_.define_method("copy", [](T& pair) -> T
            {
              return pair;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& pair) -> T
            {
              throw std::runtime_error("Cannot copy pair with non-copy constructible types");
              return pair;
            });
        }
      }

      void define_access_methods()
      {
        // Access methods
        klass_.define_method("first", [](T& pair) -> typename T::first_type&
          {
            return pair.first;
          })
        .define_method("second", [](T& pair) -> typename T::second_type&
          {
            return pair.second;
          });
      }

      void define_modify_methods()
      {
        // Access methods
        klass_.define_method("first=", [](T& pair, typename T::first_type& value) -> typename T::first_type&
        {
          if constexpr (std::is_const_v<std::remove_reference_t<std::remove_pointer_t<typename T::first_type>>>)
          {
            throw std::runtime_error("Cannot set pair.first since it is a constant");
          }
          else
          {
            pair.first = value;
            return pair.first;
          }
        })
        .define_method("second=", [](T& pair, typename T::second_type& value) -> typename T::second_type&
        {
          if constexpr (std::is_const_v<std::remove_reference_t<std::remove_pointer_t<typename T::second_type>>>)
          {
            throw std::runtime_error("Cannot set pair.second since it is a constant");
          }
          else
          {
            pair.second = value;
            return pair.second;
          }
        });
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<typename T::first_type> && detail::is_ostreamable_v<typename T::second_type>)
        {
          klass_.define_method("to_s", [](const T& pair)
            {
              std::stringstream stream;
              stream << "[" << pair.first << ", " << pair.second << "]";
              return stream.str();
            });
        }
        else
        {
          klass_.define_method("to_s", [](const T& pair)
            {
              return "[Not printable]";
            });
        }
      }

      private:
        Data_Type<T> klass_;
    };
  } // namespace

  template<typename T1, typename T2>
  Data_Type<std::pair<T1, T2>> define_pair(std::string klassName)
  {
    using Pair_T = std::pair<T1, T2>;
    using Data_Type_T = Data_Type<Pair_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(Pair_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<Pair_T>>(rb_mStd, id);
    stl::PairHelper helper(result);
    return result;
  }

  namespace detail
  {
    template<typename T1, typename T2>
    struct Type<std::pair<T1, T2>>
    {
      static bool verify()
      {
        detail::verifyType<T1>();
        detail::verifyType<T2>();

        if (!Data_Type<std::pair<T1, T2>>::is_defined())
        {
          define_pair<T1, T2>();
        }

        return true;
      }
    };
  }
}



// =========   map.hpp   =========

namespace Rice
{
  template<typename K, typename T>
  Data_Type<std::map<K, T>> define_map(std::string name = "");
}


// ---------   map.ipp   ---------
#include <map>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class MapHelper
    {
      using Key_T = typename T::key_type;
      using Mapped_T = typename T::mapped_type;
      using Value_T = typename T::value_type;
      using Reference_T = typename T::reference;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;
      using To_Ruby_T = typename detail::remove_cv_recursive_t<Mapped_T>;

    public:
      MapHelper(Data_Type<T> klass) : klass_(klass)
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
        define_pair<const Key_T, Mapped_T>();
      }

      void define_constructor()
      {
        klass_.define_constructor(Constructor<T>());
      }

      void define_copyable_methods()
      {
        if constexpr (std::is_copy_constructible_v<Value_T>)
        {
          klass_.define_method("copy", [](T& map) -> T
            {
              return map;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& map) -> T
            {
              throw std::runtime_error("Cannot copy maps with non-copy constructible types");
              return map;
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
        klass_.define_method("[]", [](const T& map, const Key_T& key) -> std::optional<Mapped_T>
          {
            auto iter = map.find(key);

            if (iter != map.end())
            {
              return iter->second;
            }
            else
            {
              return std::nullopt;
            }
          })
          .define_method("include?", [](T& map, Key_T& key) -> bool
          {
              return map.find(key) != map.end();
          })
          .define_method("keys", [](T& map) -> std::vector<Key_T>
            {
              std::vector<Key_T> result;
              std::transform(map.begin(), map.end(), std::back_inserter(result),
                [](const auto& pair)
                {
                  return pair.first;
                });

              return result;
            })
          .define_method("values", [](T& map) -> std::vector<Mapped_T>
            {
              std::vector<Mapped_T> result;
              std::transform(map.begin(), map.end(), std::back_inserter(result),
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
          klass_.define_method("value?", [](T& map, Mapped_T& value) -> bool
            {
              auto it = std::find_if(map.begin(), map.end(),
              [&value](auto& pair)
                {
                  return pair.second == value;
                });

              return it != map.end();
          });
        }
        else
        {
          klass_.define_method("value?", [](T& map, Mapped_T& value) -> bool
          {
              return false;
          });
        }

        rb_define_alias(klass_, "has_value", "value?");
      }

      void define_modify_methods()
      {
        klass_.define_method("clear", &T::clear)
          .define_method("delete", [](T& map, Key_T& key) -> std::optional<Mapped_T>
            {
              auto iter = map.find(key);

              if (iter != map.end())
              {
                Mapped_T result = iter->second;
                map.erase(iter);
                return result;
              }
              else
              {
                return std::nullopt;
              }
            })
          .define_method("[]=", [](T& map, Key_T key, Mapped_T& value) -> Mapped_T
            {
              map[key] = value;
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
        klass_.define_method("to_h", [](T& map)
        {
          VALUE result = rb_hash_new();
          std::for_each(map.begin(), map.end(), [&result](const Reference_T pair)
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
          klass_.define_method("to_s", [](const T& map)
            {
              auto iter = map.begin();

              std::stringstream stream;
              stream << "{";

              for (; iter != map.end(); iter++)
              {
                if (iter != map.begin())
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
          klass_.define_method("to_s", [](const T& map)
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
  Data_Type<std::map<Key, T>> define_map(std::string klassName)
  {
    using Map_T = std::map<Key, T>;
    using Data_Type_T = Data_Type<Map_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(Map_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<Map_T>>(rb_mStd, id);
    stl::MapHelper helper(result);
    return result;
  }

  namespace detail
  {
    template<typename Key_T, typename T>
    struct Type<std::map<Key_T, T>>
    {
      static bool verify()
      {
        Type<Key_T>::verify();
        Type<T>::verify();

        if (!Data_Type<std::map<Key_T, T>>::is_defined())
        {
          define_map<Key_T, T>();
        }

        return true;
      }
    };

    template<typename T, typename U>
    struct MapFromHash
    {
      static int convertPair(VALUE key, VALUE value, VALUE user_data)
      {
        std::map<T, U>* result = (std::map<T, U>*)(user_data);

        // This method is being called from Ruby so we cannot let any C++ 
        // exceptions propogate back to Ruby
        return cpp_protect([&]
        {
          result->operator[](From_Ruby<T>().convert(key)) = From_Ruby<U>().convert(value);
          return ST_CONTINUE;
        });
      }

      static std::map<T, U> convert(VALUE value)
      {
        std::map<T, U> result;
        VALUE user_data = (VALUE)(&result);

        // MSVC needs help here, but g++ does not
        using Rb_Hash_ForEach_T = void(*)(VALUE, int(*)(VALUE, VALUE, VALUE), VALUE);
        detail::protect<Rb_Hash_ForEach_T>(rb_hash_foreach, value, convertPair, user_data);

        return result;
      }
    };

    template<typename T, typename U>
    class From_Ruby<std::map<T, U>>
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
            return Data_Type<std::map<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::map<T, U> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped map (hopefully!)
            return *detail::unwrap<std::map<T, U>>(value, Data_Type<std::map<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby hash and the mapped type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              return MapFromHash<T, U>::convert(value);
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::map");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<typename T, typename U>
    class From_Ruby<std::map<T, U>&>
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
            return Data_Type<std::map<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_HASH:
            return Convertible::Cast;
            break;
          default:
            return Convertible::None;
        }
      }

      std::map<T, U>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped map (hopefully!)
            return *detail::unwrap<std::map<T, U>>(value, Data_Type<std::map<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby array and the map type is copyable
            if constexpr (std::is_default_constructible_v<std::map<T, U>>)
            {
              this->converted_ = MapFromHash<T, U>::convert(value);
              return this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::map");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
      std::map<T, U> converted_;
    };

    template<typename T, typename U>
    class From_Ruby<std::map<T, U>*>
    {
    public:
      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Data_Type<std::map<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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

      std::map<T, U>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped map (hopefully!)
            return detail::unwrap<std::map<T, U>>(value, Data_Type<std::map<T, U>>::ruby_data_type(), false);
          }
          case RUBY_T_HASH:
          {
            // If this an Ruby array and the map type is copyable
            if constexpr (std::is_default_constructible_v<U>)
            {
              this->converted_ = MapFromHash<T, U>::convert(value);
              return &this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::map");
          }
        }
      }

    private:
      std::map<T, U> converted_;
    };
  }
}

// =========   monostate.hpp   =========


// ---------   monostate.ipp   ---------
#include <variant>

namespace Rice::detail
{
  template<>
  struct Type<std::monostate>
  {
    constexpr static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<std::monostate>
  {
  public:
    VALUE convert(const std::monostate& _)
    {
      return Qnil;
    }
  };

  template<>
  class To_Ruby<std::monostate&>
  {
  public:
    static VALUE convert(const std::monostate& data, bool takeOwnership = false)
    {
      return Qnil;
    }
  };

  template<>
  class From_Ruby<std::monostate>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return Convertible::None;
    }

    std::monostate convert(VALUE value)
    {
      return std::monostate();
    }
  };

  template<>
  class From_Ruby<std::monostate&>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return Convertible::None;
    }

    std::monostate& convert(VALUE value)
    {
      return this->converted_;
    }
    
  private:
    std::monostate converted_ = std::monostate();
  };
}


// =========   multimap.hpp   =========

#include <map>

namespace Rice
{
  template<typename K, typename T>
  Data_Type<std::multimap<K, T>> define_multimap(std::string name = "");
}


// ---------   multimap.ipp   ---------
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
        this->define_constructor();
        this->define_copyable_methods();
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

      void define_constructor()
      {
        klass_.define_constructor(Constructor<T>());
      }

      void define_copyable_methods()
      {
        if constexpr (std::is_copy_constructible_v<Value_T>)
        {
          klass_.define_method("copy", [](T& multimap) -> T
            {
              return multimap;
            });
        }
        else
        {
          klass_.define_method("copy", [](T& multimap) -> T
            {
              throw std::runtime_error("Cannot copy multimaps with non-copy constructible types");
              return multimap;
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
        klass_.
          define_method("[]", [](const T& multimap, const Key_T& key) -> Array
          {
            Array result;
            auto range = multimap.equal_range(key);

            for (auto iter = range.first; iter != range.second; iter++)
            {
              result.push<Mapped_T>(iter->second);
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
          klass_.define_method("value?", [](T& multimap, Mapped_T& value) -> bool
            {
              auto it = std::find_if(multimap.begin(), multimap.end(),
              [&value](auto& pair)
                {
                  return pair.second == value;
                });

              return it != multimap.end();
          });
        }
        else
        {
          klass_.define_method("value?", [](T& multimap, Mapped_T& value) -> bool
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
              stream << "<" << detail::rubyClassName(detail::typeName(typeid(T))) << ":";
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
          klass_.define_method("to_s", [](const T& multimap)
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
      std::string typeName = detail::typeName(typeid(MultiMap_T));
      klassName = detail::rubyClassName(typeName);
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
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
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
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
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
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::multimap");
          }
        }
      }

    private:
      std::multimap<T, U> converted_;
    };
  }
}

// =========   set.hpp   =========

namespace Rice
{
  template<typename T>
  Data_Type<std::set<T>> define_set(std::string klassName = "");
}


// ---------   set.ipp   ---------
#include <set>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class SetHelper
    {
      // We do NOT use Reference_T and instead use Parameter_T to avoid the weirdness
      // of std::set<bool>. Reference_T is actually a proxy class that we do not
      // want to have to register with Rice nor do we want to pass it around.
      using Key_T = typename T::key_type;
      using Value_T = typename T::value_type;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;
      // For To_Ruby_T however we do need to use reference type because this is what
      // will be passed by an interator to To_Ruby#convert
      using Reference_T = typename T::reference;
      using Parameter_T = std::conditional_t<std::is_pointer_v<Value_T>, Value_T, Value_T&>;
      using To_Ruby_T = detail::remove_cv_recursive_t<typename T::reference>;

    public:
      SetHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->define_constructors();
        this->define_capacity_methods();
        this->define_comparable_methods();
        this->define_modify_methods();
        this->define_operators();
        this->define_enumerable();
        this->define_to_array();
        this->define_to_s();
      }

    private:

      void define_constructors()
      {
        klass_.define_constructor(Constructor<T>())
              .define_constructor(Constructor<T, const T&>());
      }

      void define_capacity_methods()
      {
        klass_.define_method("empty?", &T::empty)
          .define_method("max_size", &T::max_size)
          .define_method("size", &T::size);
        
        rb_define_alias(klass_, "count", "size");
        rb_define_alias(klass_, "length", "size");
      }

      void define_comparable_methods()
      {
        klass_
          .define_method("include?", [](T& self, const Key_T element) -> bool
          {
            auto iter = self.find(element);
            return iter != self.end();
          })
          .define_method("count", [](T& self, const Key_T element) -> Size_T
          {
            return self.count(element);
          });
      }

      void define_modify_methods()
      {
        klass_
          .define_method("clear", &T::clear)
          .define_method("delete", [](T& self, const Key_T key) -> T&
          {
            self.erase(key);
            return self;
          })
          .define_method("insert", [](T& self, const Value_T value) -> T&
          {
            self.insert(value);
            return self;
          })
          .define_method("merge", [](T& self, T& other) -> T&
          {
            self.merge(other);
            return self;
          });

        rb_define_alias(klass_, "erase", "delete");
      }

      void define_operators()
      {
        klass_
          .define_method("<<", [](T& self, const Value_T value) -> T&
          {
            self.insert(value);
            return self;
          })
          .define_method("==", [](const T& self, const T& other) -> bool
          {
            if constexpr (detail::is_comparable_v<Value_T>)
            {
              return self == other;
            }
            else
            {
              return false;
            }
          })
          .define_method("&", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_intersection(self.begin(), self.end(),
                                  other.begin(), other.end(),
                                  std::inserter(result, result.begin()));

            return result;
          })
          .define_method("|", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_union(self.begin(), self.end(),
                           other.begin(), other.end(),
                           std::inserter(result, result.begin()));

            return result;
          })
          .define_method("-", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_difference(self.begin(), self.end(),
                                other.begin(), other.end(),
                                std::inserter(result, result.begin()));

            return result;
          })
          .define_method("^", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_symmetric_difference(self.begin(), self.end(),
                                          other.begin(), other.end(),
                                          std::inserter(result, result.begin()));

            return result;
          })
          .define_method("<", [](const T& self, const T& other) -> bool
          {
            return std::includes(other.begin(), other.end(),
                                 self.begin(), self.end());
          })
          .define_method(">", [](const T& self, const T& other) -> bool
          {
            return std::includes(self.begin(), self.end(),
                                 other.begin(), other.end());
          });
        
          rb_define_alias(klass_, "eql?", "==");
          rb_define_alias(klass_, "intersection", "&");
          rb_define_alias(klass_, "union", "|");
          rb_define_alias(klass_, "difference", "-");
          rb_define_alias(klass_, "proper_subset?", "<");
          rb_define_alias(klass_, "subset?", "<");
          rb_define_alias(klass_, "proper_superset?", ">");
          rb_define_alias(klass_, "superset?", ">");
      }

      void define_enumerable()
      {
        // Add enumerable support
        klass_.template define_iterator<typename T::iterator(T::*)() const>(&T::begin, &T::end);
      }

      void define_to_array()
      {
        // Add enumerable support
        klass_.define_method("to_a", [](T& self) -> VALUE
        {
          Array array;
          for (const Value_T& element: self)
          {
            array.push(element);
          }

          return array.value();
        }, Return().setValue());
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<Value_T>)
        {
          klass_.define_method("to_s", [](const T& self)
          {
            auto iter = self.begin();
            auto finish = self.end();

            std::stringstream stream;
            stream << "<" << detail::rubyClassName(detail::typeName(typeid(T))) << ":";
            stream << "{";

            for (; iter != finish; iter++)
            {
              if (iter == self.begin())
              {
                stream << *iter;
              }
              else
              {
                stream << ", " << *iter;
              }
            }

            stream << "}>";
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
  Data_Type<std::set<T>> define_set(std::string klassName)
  {
    using Set_T = std::set<T>;
    using Data_Type_T = Data_Type<Set_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(Set_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<Set_T>>(rb_mStd, id);
    stl::SetHelper helper(result);
    return result;
  }

  namespace detail
  {
    // Helper method - maybe someday create a C++ Ruby set wrapper
    template<typename T>
    std::set<T> toSet(VALUE rubySet)
    {
      using Function_T = VALUE(*)(VALUE, ID, int, const VALUE*, rb_block_call_func_t, VALUE);
      static Identifier identifier("each");

      std::set<T> result;
      auto block = [&result](const typename std::set<T>::value_type element) -> VALUE
      {
        result.insert(element);
        return Qnil;
      };
      
      using NativeFunction_T = NativeFunction<void, decltype(block), false>;

      // It is ok to use the address of native because it will remain valid while we iterate the set
      NativeFunction_T native(block);
      detail::protect<Function_T>(rb_block_call, rubySet, identifier.id(), 0, nullptr, NativeFunction_T::procEntry, (VALUE)&native);
      
      return result;
    }

    template<typename T>
    struct Type<std::set<T>>
    {
      static bool verify()
      {
        Type<intrinsic_type<T>>::verify();

        if (!Data_Type<std::set<T>>::is_defined())
        {
          define_set<T>();
        }

        return true;
      }
    };

    template<typename T>
    class From_Ruby<std::set<T>>
    {
    private:
      static inline std::string setName = "Set";

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
            return Data_Type<std::set<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return Convertible::Cast;
            }
          }
          default:
            return Convertible::None;
        }
      }

      std::set<T> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped self (hopefully!)
            return *detail::unwrap<std::set<T>>(value, Data_Type<std::set<T>>::ruby_data_type(), false);
          }
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return toSet<T>(value);
            }
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<typename T>
    class From_Ruby<std::set<T>&>
    {
    private:
      static inline std::string setName = "Set";

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
            return Data_Type<std::set<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return Convertible::Cast;
            }
          }
          default:
            return Convertible::None;
        }
      }

      std::set<T>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped self (hopefully!)
            return *detail::unwrap<std::set<T>>(value, Data_Type<std::set<T>>::ruby_data_type(), false);
          }
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              // If this an Ruby array and the vector type is copyable
              if constexpr (std::is_default_constructible_v<T>)
              {
                this->converted_ = toSet<T>(value);
                return this->converted_;
              }
            }
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
      std::set<T> converted_;
    };

    template<typename T>
    class From_Ruby<std::set<T>*>
    {
    private:
      static inline std::string setName = "Set";
    public:
      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Data_Type<std::set<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_NIL:
            return Convertible::Exact;
            break;
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return Convertible::Cast;
            }
          }
          default:
            return Convertible::None;
        }
      }

      std::set<T>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped self (hopefully!)
            return detail::unwrap<std::set<T>>(value, Data_Type<std::set<T>>::ruby_data_type(), false);
          }
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              // If this an Ruby array and the vector type is copyable
              if constexpr (std::is_default_constructible_v<T>)
              {
                this->converted_ = toSet<T>(value);
                return &this->converted_;
              }
            }
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
        }
      }

    private:
      std::set<T> converted_;
    };
  }
}


// =========   shared_ptr.hpp   =========

namespace Rice::detail
{
  template<typename T>
  class Wrapper<std::shared_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(const std::shared_ptr<T>& data);
    ~Wrapper();
    void* get() override;
    std::shared_ptr<T>& data();

  private:
    std::shared_ptr<T> data_;
  };
}

namespace Rice
{
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName = "");
}


// ---------   shared_ptr.ipp   ---------
#include <memory>

// --------- Enable creation of std::shared_ptr from Ruby ---------
namespace Rice
{
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName)
  {
    using SharedPtr_T = std::shared_ptr<T>;
    using Data_Type_T = Data_Type<SharedPtr_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(SharedPtr_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<SharedPtr_T>>(rb_mStd, id).
      define_constructor(Constructor<SharedPtr_T, typename SharedPtr_T::element_type*>(), Arg("value").takeOwnership());

    return result;
  }
}

// --------- Wrapper ---------
namespace Rice::detail
{
  template<typename T>
  inline Wrapper<std::shared_ptr<T>>::Wrapper(const std::shared_ptr<T>& data)
    : data_(data)
  {
  }

  template<typename T>
  inline Wrapper<std::shared_ptr<T>>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template<typename T>
  inline void* Wrapper<std::shared_ptr<T>>::get()
  {
    return (void*)this->data_.get();
  }

  template<typename T>
  inline std::shared_ptr<T>& Wrapper<std::shared_ptr<T>>::data()
  {
    return data_;
  }
}

// --------- Type/To_Ruby/From_Ruby ---------
namespace Rice::detail
{
  template<typename T>
  struct Type<std::shared_ptr<T>>
  {
    static bool verify()
    {
      return Type<T>::verify();
    }
  };

  template <typename T>
  class To_Ruby<std::shared_ptr<T>>
  {
  public:
    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return detail::wrap(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
      else
      {
        return detail::wrap<std::shared_ptr<T>>(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
    }

    VALUE convert(std::shared_ptr<T>&& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return detail::wrap(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
      else
      {
        return detail::wrap<std::shared_ptr<T>>(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
    }
  };

  template <typename T>
  class From_Ruby<std::shared_ptr<T>>
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
        default:
          return Convertible::None;
      }
    }

    std::shared_ptr<T> convert(VALUE value)
    {
      // Get the wrapper
      WrapperBase* wrapperBase = detail::getWrapper(value);

      // Was this shared_ptr created by the user from Ruby? If so it will
      // be wrapped as a pointer, std::shared_ptr<T>*. In the case just
      // return the shared pointer
      if (dynamic_cast<Wrapper<std::shared_ptr<T>*>*>(wrapperBase))
      {
        // Use unwrap to validate the underlying wrapper is the correct type
        std::shared_ptr<T>* ptr = unwrap<std::shared_ptr<T>>(value, Data_Type<std::shared_ptr<T>>::ruby_data_type(), false);
        return *ptr;
      }
      else if constexpr (std::is_fundamental_v<T>)
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
      else
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
    }
  private:
    Arg* arg_ = nullptr;
  };

  template <typename T>
  class To_Ruby<std::shared_ptr<T>&>
  {
  public:
    VALUE convert(std::shared_ptr<T>& data)
    {
      if constexpr (std::is_fundamental_v<T>)
      {
        return detail::wrap(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
      else
      {
        return detail::wrap<std::shared_ptr<T>>(Data_Type<T>::klass(), Data_Type<T>::ruby_data_type(), data, true);
      }
    }
  };

  template <typename T>
  class From_Ruby<std::shared_ptr<T>&>
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
        default:
          return Convertible::None;
      }
    }

    std::shared_ptr<T>& convert(VALUE value)
    {
      // Get the wrapper
      WrapperBase* wrapperBase = detail::getWrapper(value);

      // Was this shared_ptr created by the user from Ruby? If so it will
      // be wrapped as a pointer, std::shared_ptr<T>*. In the case just
      // return the shared pointer
      if (dynamic_cast<Wrapper<std::shared_ptr<T>*>*>(wrapperBase))
      {
        // Use unwrap to validate the underlying wrapper is the correct type
        std::shared_ptr<T>* ptr = unwrap<std::shared_ptr<T>>(value, Data_Type<std::shared_ptr<T>>::ruby_data_type(), false);
        return *ptr;
      }
      else if constexpr (std::is_fundamental_v<T>)
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
      else
      {
        // Get the wrapper again to validate T's type
        Wrapper<std::shared_ptr<T>>* wrapper = getWrapper<Wrapper<std::shared_ptr<T>>>(value, Data_Type<T>::ruby_data_type());
        return wrapper->data();
      }
    }

  private:
    Arg* arg_ = nullptr;
  };
}


// =========   tuple.hpp   =========


// ---------   tuple.ipp   ---------
#include <tuple>

namespace Rice::detail
{
  template<typename...Types>
  struct Type<std::tuple<Types...>>
  {
    using Tuple_T = std::tuple<Types...>;

    template<std::size_t... I>
    constexpr static bool verifyTypes(std::index_sequence<I...>& indices)
    {
      return (Type<std::tuple_element_t<I, Tuple_T>>::verify() && ...);
    }

    template<std::size_t... I>
    constexpr static bool verify()
    {
      auto indices = std::make_index_sequence<std::tuple_size_v<std::tuple<Types...>>>{};
      return verifyTypes(indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::tuple<Types...>>
  {
  public:
    static VALUE convert(const std::tuple<Types...>& data, bool takeOwnership = false)
    {
      Array result;

      for_each_tuple(data, [&](auto element)
      {
        using Element_T = decltype(element);
        result.push<Element_T>((Element_T)element);
      });

      return result.value();
    }
  };

  template<typename...Types>
  class To_Ruby<std::tuple<Types...>&>
  {
  public:
    static VALUE convert(const std::tuple<Types...>& data, bool takeOwnership = false)
    {
      Array result;

      for_each_tuple(data, [&](auto& value)
        {
          VALUE element = detail::To_Ruby<decltype(value)>().convert(value);
          result.push(element);
        });

      return result.value();
    }
  };

  template<typename...Types>
  class From_Ruby<std::tuple<Types...>>
  {
  public:
    using Tuple_T = std::tuple<Types...>;

    template<std::size_t... I>
    constexpr static bool verifyTypes(Array& array, std::index_sequence<I...>& indices)
    {
      return (Type<std::tuple_element_t<I, Tuple_T>>::verify() && ...);
    }

    Convertible is_convertible(VALUE value)
    {
      Convertible result = Convertible::None;

      // The ruby value must be an array of the correct size
      if (rb_type(value) != RUBY_T_ARRAY || Array(value).size() != std::tuple_size_v<Tuple_T>)
      {
        return result;
      }
      
      // Now check that each tuple type is convertible
      Array array(value);
      int i = 0;
      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          result = result | fromRuby.is_convertible(array[i].value());
          i++;
        });

      return result;
    }

    template <std::size_t... I>
    std::tuple<Types...> convertInternal(Array array, std::index_sequence<I...>& indices)
    {
      return std::forward_as_tuple(std::get<I>(this->fromRubys_).convert(array[I].value())...);
    }

    std::tuple<Types...> convert(VALUE value)
    {
      Array array(value);
      auto indices = std::make_index_sequence<std::tuple_size_v<Tuple_T>>{};
      return convertInternal(array, indices);
    }

  private:
    // Possible converters we could use for this variant
    using From_Ruby_Ts = std::tuple<From_Ruby<Types>...>;
    From_Ruby_Ts fromRubys_;
  };

/*  template<typename...Types>
  class From_Ruby<std::tuple<Types...>&> : public From_Ruby<std::tuple<Types...>>
  {
  public:
    std::tuple<Types...>& convert(VALUE value)
    {
      int index = this->figureIndex(value);
      this->converted_ = this->convertInternal(value, index);
      return this->converted_;
    }

  private:
    std::tuple<Types...> converted_;
  };*/
}


// =========   type_index.hpp   =========


// ---------   type_index.ipp   ---------
#include <typeindex>

namespace Rice::stl
{
  inline Data_Type<std::type_index> define_type_index()
  {
    Module rb_mStd = define_module("Std");
    return define_class_under<std::type_index>(rb_mStd, "TypeIndex").
      define_constructor(Constructor<std::type_index, const std::type_info&>()).
      define_method("hash_code", &std::type_index::hash_code).
      define_method("name", &std::type_index::name);
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::type_index>
  {
    static bool verify()
    {
      if (!detail::Registries::instance.types.isDefined<std::type_index>())
      {
        stl::define_type_index();
      }

      return true;
    }
  };
}


// =========   type_info.hpp   =========


// ---------   type_info.ipp   ---------
#include <typeinfo>

namespace Rice::stl
{
  inline Data_Type<std::type_info> define_type_info()
  {
    Module rb_mStd = define_module("Std");
    return define_class_under<std::type_info>(rb_mStd, "TypeInfo").
      define_method("hash_code", &std::type_info::hash_code).
      define_method("name", &std::type_info::name);
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::type_info>
  {
    static inline bool verify()
    {
      if (!detail::Registries::instance.types.isDefined<std::type_info>())
      {
        stl::define_type_info();
      }

      return true;
    }
  };
}


// =========   variant.hpp   =========


// ---------   variant.ipp   ---------
#include <variant>

namespace Rice::detail
{
  template<typename...Types>
  struct Type<std::variant<Types...>>
  {
    using Tuple_T = std::tuple<Types...>;

    template<std::size_t... I>
    constexpr static bool verifyTypes(std::index_sequence<I...>& indices)
    {
      return (Type<std::tuple_element_t<I, Tuple_T>>::verify() && ...);
    }

    template<std::size_t... I>
    constexpr static bool verify()
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return verifyTypes(indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::variant<Types...>>
  {
  public:

    template<typename T>
    static VALUE convertElement(std::variant<Types...>& data, bool takeOwnership)
    {
      return To_Ruby<T>().convert(std::forward<T>(std::get<T>(data)));
    }

    template<typename U, std::size_t... I>
    static VALUE convertIterator(U& data, bool takeOwnership, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;
      
      /* This is a fold expression. In pseudo code:
       
        for (type in variant.types)
        {
          if (variant.has_value<type>())
            return ToRuby<type>().convert(variant.getValue<type>)
        }

        The list of variant types is stored in Tuple_T. The number of types is stored in I.
        Starting with index 0, get the variant type using td::tuple_element_t<I, Tuple_T>>.
        Next check if the variant has a value for that type using std::holds_alternative<T>. 
        If yes, then call convertElement and save the return value to result. Then use the 
        comma operator to return true to the fold expression. If the variant does not have
        a value for the type then return false. 
        
        The fold operator is or (||). If an index returns false, then the next index is evaluated
        up until I. 
        
        Code inspired by https://www.foonathan.net/2020/05/fold-tricks/ */

      VALUE result = Qnil;

      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wunused-value"
      #endif

      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
               (result = convertElement<std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);
      
      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic pop
      #endif

      return result;
    }

    template<typename U>
    static VALUE convert(U& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }

    template<typename U>
    static VALUE convert(U&& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class To_Ruby<std::variant<Types...>&>
  {
  public:
    template<typename T>
    static VALUE convertElement(std::variant<Types...>& data, bool takeOwnership)
    {
      return To_Ruby<T>().convert(std::forward<T>(std::get<T>(data)));
    }

    template<std::size_t... I>
    static VALUE convertIterator(std::variant<Types...>& data, bool takeOwnership, std::index_sequence<I...>& indices)
    {
      // Create a tuple of the variant types so we can look over the tuple's types
      using Tuple_T = std::tuple<Types...>;

      // See comments above for explanation of this code
      VALUE result = Qnil;

      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wunused-value"
      #endif

      ((std::holds_alternative<std::tuple_element_t<I, Tuple_T>>(data) ?
        (result = convertElement<std::tuple_element_t<I, Tuple_T>>(data, takeOwnership), true) : false) || ...);

      #if defined(__GNUC__) || defined(__clang__)
      #pragma GCC diagnostic pop
      #endif

      return result;
    }

    static VALUE convert(std::variant<Types...>& data, bool takeOwnership = false)
    {
      auto indices = std::make_index_sequence<std::variant_size_v<std::variant<Types...>>>{};
      return convertIterator(data, takeOwnership, indices);
    }
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      Convertible result = Convertible::None;

      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          result = result | fromRuby.is_convertible(value);
        });

      return result;
    }

    // This method search through a variant's types to figure out which one the
    // currently Ruby value best matches. It then returns the index of the type.
    int figureIndex(VALUE value)
    {
      int i = 0;
      int index = -1;
      Convertible foundConversion = Convertible::None;

      for_each_tuple(this->fromRubys_,
        [&](auto& fromRuby)
        {
          Convertible isConvertible = fromRuby.is_convertible(value);

          if (isConvertible > foundConversion)
          {
            index = i;
            foundConversion = isConvertible;
          }
          i++;
        });

      if (index == -1)
      {
        rb_raise(rb_eArgError, "Could not find converter for variant");
      }

      return index;
    }

    /* This method loops over each type in the variant, creates a From_Ruby converter,
       and then check if the converter can work with the provided Rby value (it checks
       the type of the Ruby object to see if it matches the variant type).
       If yes, then the converter runs. If no, then the method recursively calls itself
       increasing the index.

       We use recursion, with a constexpr, to avoid having to instantiate an instance
       of the variant to store results from a fold expression like the To_Ruby code
       does above. That allows us to process variants with non default constructible
       arguments like std::reference_wrapper. */
    template <std::size_t I = 0>
    std::variant<Types...> convertInternal(VALUE value, int index)
    {
      if constexpr (I < std::variant_size_v<std::variant<Types...>>)
      {
        if (I == index)
        {
          auto fromRuby = std::get<I>(this->fromRubys_);
          return fromRuby.convert(value);
        }
        else
        {
          return convertInternal<I + 1>(value, index);
        }
      }
      rb_raise(rb_eArgError, "Could not find converter for variant");
    }

    std::variant<Types...> convert(VALUE value)
    {
      int index = this->figureIndex(value);
      return this->convertInternal(value, index);
    }

  private:
    // Possible converters we could use for this variant
    using From_Ruby_Ts = std::tuple<From_Ruby<Types>...>;
    From_Ruby_Ts fromRubys_;
  };

  template<typename...Types>
  class From_Ruby<std::variant<Types...>&> : public From_Ruby<std::variant<Types...>>
  {
  public:
    std::variant<Types...>& convert(VALUE value)
    {
      int index = this->figureIndex(value);
      this->converted_ = this->convertInternal(value, index);
      return this->converted_;
    }

  private:
    std::variant<Types...> converted_;
  };
}


// =========   unique_ptr.hpp   =========

namespace Rice::detail
{
  template<typename T>
  class Wrapper<std::unique_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(std::unique_ptr<T>&& data);
    ~Wrapper();
    void* get() override;
    std::unique_ptr<T>& data();

  private:
    std::unique_ptr<T> data_;
  };
}


// ---------   unique_ptr.ipp   ---------
#include <memory>

namespace Rice::detail
{
  template<typename T>
  inline Wrapper<std::unique_ptr<T>>::Wrapper(std::unique_ptr<T>&& data)
    : data_(std::move(data))
  {
  }

  template<typename T>
  inline Wrapper<std::unique_ptr<T>>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template<typename T>
  inline void* Wrapper<std::unique_ptr<T>>::get()
  {
    return (void*)this->data_.get();
  }

  template<typename T>
  inline std::unique_ptr<T>& Wrapper<std::unique_ptr<T>>::data()
  {
    return data_;
  }

  template <typename T>
  class To_Ruby<std::unique_ptr<T>>
  {
  public:
    VALUE convert(std::unique_ptr<T>& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }

    VALUE convert(std::unique_ptr<T>&& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, data, true);
    }
  };

  template <typename T>
  class To_Ruby<std::unique_ptr<T>&>
  {
  public:
    VALUE convert(std::unique_ptr<T>& data)
    {
      std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType<T>(*data);
      return detail::wrap<std::unique_ptr<T>>(rubyTypeInfo.first, rubyTypeInfo.second, std::move(data), true);
    }
  };

  template <typename T>
  class From_Ruby<std::unique_ptr<T>>
  {
  public:
    Wrapper<std::unique_ptr<T>>* is_same_smart_ptr(VALUE value)
    {
      WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
      return dynamic_cast<Wrapper<std::unique_ptr<T>>*>(wrapper);
    }

    Convertible is_convertible(VALUE value)
    {
      if (!is_same_smart_ptr(value))
        return Convertible::None;

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::unique_ptr<T> convert(VALUE value)
    {
      Wrapper<std::unique_ptr<T>>* wrapper = is_same_smart_ptr(value);
      if (!wrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return std::move(wrapper->data());
    }
  };

  template <typename T>
  class From_Ruby<std::unique_ptr<T>&>
  {
  public:
    Wrapper<std::unique_ptr<T>>* is_same_smart_ptr(VALUE value)
    {
      WrapperBase* wrapper = detail::getWrapper(value, Data_Type<T>::ruby_data_type());
      return dynamic_cast<Wrapper<std::unique_ptr<T>>*>(wrapper);
    }

    Convertible is_convertible(VALUE value)
    {
      if (!is_same_smart_ptr(value))
        return Convertible::None;

      switch (rb_type(value))
      {
        case RUBY_T_DATA:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    std::unique_ptr<T>& convert(VALUE value)
    {
      Wrapper<std::unique_ptr<T>>* wrapper = is_same_smart_ptr(value);
      if (!wrapper)
      {
        std::string message = "Invalid smart pointer wrapper";
        throw std::runtime_error(message.c_str());
      }
      return wrapper->data();
    }
  };

  template<typename T>
  struct Type<std::unique_ptr<T>>
  {
    static bool verify()
    {
      return Type<T>::verify();
    }
  };
}


// =========   unordered_map.hpp   =========

namespace Rice
{
  template<typename Key, typename T>
  Data_Type<std::unordered_map<Key, T>> define_unordered_map(std::string name = "");
}


// ---------   unordered_map.ipp   ---------
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
        define_pair<const Key_T, T>();
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

  template<typename Key, typename T>
  Data_Type<std::unordered_map<Key, T>> define_unordered_map(std::string klassName)
  {
    using UnorderedMap_T = std::unordered_map<Key, T>;
    using Data_Type_T = Data_Type<UnorderedMap_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(UnorderedMap_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<UnorderedMap_T>>(rb_mStd, id);
    stl::UnorderedMapHelper helper(result);
    return result;
  }

  namespace detail
  {
    template<typename Key_T, typename T>
    struct Type<std::unordered_map<Key_T, T>>
    {
      static bool verify()
      {
        Type<Key_T>::verify();
        Type<T>::verify();

        if (!Data_Type<std::unordered_map<Key_T, T>>::is_defined())
        {
          define_unordered_map<Key_T, T>();
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
            return Data_Type<std::unordered_map<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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
            return Data_Type<std::unordered_map<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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
            return Data_Type<std::unordered_map<T, U>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
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

// =========   vector.hpp   =========

namespace Rice
{
  template<typename T>
  Data_Type<std::vector<T>> define_vector(std::string name = "" );
}


// ---------   vector.ipp   ---------
#include <vector>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class VectorHelper
    {
      // We do NOT use Reference_T and instead use Parameter_T to avoid the weirdness
      // of std::vector<bool>. Reference_T is actually a proxy class that we do not
      // want to have to register with Rice nor do we want to pass it around.
      using Value_T = typename T::value_type;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;
      // For To_Ruby_T however we do need to use reference type because this is what
      // will be passed by an interator to To_Ruby#convert
      using Reference_T = typename T::reference;
      using Parameter_T = std::conditional_t<std::is_pointer_v<Value_T>, Value_T, Value_T&>;
      using To_Ruby_T = detail::remove_cv_recursive_t<typename T::reference>;

    public:
      VectorHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->define_constructors();
        this->define_constructable_methods();
        this->define_capacity_methods();
        this->define_access_methods();
        this->define_comparable_methods();
        this->define_modify_methods();
        this->define_enumerable();
        this->define_to_array();
        this->define_to_s();
      }

    private:

      // Helper method to translate Ruby indices to vector indices
      Difference_T normalizeIndex(Size_T size, Difference_T index, bool enforceBounds = false)
      {
        // Negative indices mean count from the right. Note that negative indices
        // wrap around!
        if (index < 0)
        {
          index = ((-index) % size);
          index = index > 0 ? size - index : index;
        }

        if (enforceBounds && (index < 0 || index >= (Difference_T)size))
        {
          throw std::out_of_range("Invalid index: " + std::to_string(index));
        }

        return index;
      };

      void define_constructors()
      {
        klass_.define_constructor(Constructor<T>())
              .define_constructor(Constructor<T, Size_T, const Parameter_T>())
              .define_constructor(Constructor<T, const T&>());

        if constexpr (std::is_default_constructible_v<Value_T>)
        {
          klass_.define_constructor(Constructor<T, Size_T>());
        }

        // Allow creation of a vector from a Ruby Array
        klass_.define_method("initialize", [](VALUE self, Array array) -> void
        {
          // Create a new vector from the array
          T* data = new T();
          data->reserve(array.size());

          detail::From_Ruby<Value_T> fromRuby;

          for (long i = 0; i < array.size(); i++)
          {
            VALUE element = detail::protect(rb_ary_entry, array, i);
            data->push_back(fromRuby.convert(element));
          }

          // Wrap the vector
          detail::wrapConstructed<T>(self, Data_Type<T>::ruby_data_type(), data, true);
        });
      }

      void define_constructable_methods()
      {
        if constexpr (std::is_default_constructible_v<Value_T> && std::is_same_v<Value_T, bool>)
        {
          klass_.define_method("resize", static_cast<void (T::*)(const size_t, bool)>(&T::resize));
        }
        else if constexpr (std::is_default_constructible_v<Value_T>)
        {
          klass_.define_method("resize", static_cast<void (T::*)(const size_t)>(&T::resize));
        }
        else
        {
          klass_.define_method("resize", [](const T& vector, Size_T newSize)
              {
                // Do nothing
              });
        }
      }

      void define_capacity_methods()
      {
        klass_.define_method("empty?", &T::empty)
          .define_method("capacity", &T::capacity)
          .define_method("max_size", &T::max_size)
          .define_method("reserve", &T::reserve)
          .define_method("size", &T::size);
        
        rb_define_alias(klass_, "count", "size");
        rb_define_alias(klass_, "length", "size");
        //detail::protect(rb_define_alias, klass_, "count", "size");
        //detail::protect(rb_define_alias, klass_, "length", "size");
      }

      void define_access_methods()
      {
        // Access methods
        klass_.define_method("first", [](const T& vector) -> std::optional<Value_T>
        {
          if (vector.size() > 0)
          {
            return vector.front();
          }
          else
          {
            return std::nullopt;
          }
        })
        .define_method("last", [](const T& vector) -> std::optional<Value_T>
          {
            if (vector.size() > 0)
            {
              return vector.back();
            }
            else
            {
              return std::nullopt;
            }
          })
        .define_method("[]", [this](const T& vector, Difference_T index) -> std::optional<Value_T>
          {
            index = normalizeIndex(vector.size(), index);
            if (index < 0 || index >= (Difference_T)vector.size())
            {
              return std::nullopt;
            }
            else
            {
              return vector[index];
            }
          })
        .define_method("[]", [this](const T& vector, Difference_T start, Difference_T length) -> VALUE
          {
            start = normalizeIndex(vector.size(), start);
            if (start < 0 || start >= (Difference_T)vector.size())
            {
              return rb_ary_new();
            }
            else
            {
              auto begin = vector.begin() + start;

              // Ruby does not throw an exception when the length is too long
              Difference_T size = (Difference_T)vector.size();
              if (start + length > size)
              {
                length = size - start;
              }

              auto finish = vector.begin() + start + length;
              T slice(begin, finish);

              VALUE result = rb_ary_new();
              std::for_each(slice.begin(), slice.end(), [&result](const Reference_T element)
              {
                VALUE value = detail::To_Ruby<Parameter_T>().convert(element);
                rb_ary_push(result, value);
              });

              return result;
            }
          }, Return().setValue());

          if constexpr (!std::is_same_v<Value_T, bool>)
          {
            define_buffer<Value_T>();
            define_buffer<Value_T*>();
            klass_.template define_method<Value_T*(T::*)()>("data", &T::data);
          }

          rb_define_alias(klass_, "at", "[]");
      }

      // Methods that require Value_T to support operator==
      void define_comparable_methods()
      {
        if constexpr (detail::is_comparable_v<Value_T>)
        {
          klass_.define_method("delete", [](T& vector, Parameter_T element) -> std::optional<Value_T>
            {
              auto iter = std::find(vector.begin(), vector.end(), element);
              if (iter == vector.end())
              {
                return std::nullopt;
              }
              else
              {
                Value_T result = *iter;
                vector.erase(iter);
                return result;
              }
            })
          .define_method("include?", [](T& vector, Parameter_T element)
            {
              return std::find(vector.begin(), vector.end(), element) != vector.end();
            })
          .define_method("index", [](T& vector, Parameter_T element) -> std::optional<Difference_T>
            {
              auto iter = std::find(vector.begin(), vector.end(), element);
              if (iter == vector.end())
              {
                return std::nullopt;
              }
              else
              {
                return iter - vector.begin();
              }
            });
        }
        else
        {
          klass_.define_method("delete", [](T& vector, Parameter_T element) -> std::optional<Value_T>
            {
              return std::nullopt;
            })
          .define_method("include?", [](const T& vector, Parameter_T element)
            {
              return false;
            })
          .define_method("index", [](const T& vector, Parameter_T element) -> std::optional<Difference_T>
            {
              return std::nullopt;
            });
        }
      }

      void define_modify_methods()
      {
        klass_.define_method("clear", &T::clear)
          .define_method("delete_at", [](T& vector, const size_t& pos)
            {
              auto iter = vector.begin() + pos;
              Value_T result = *iter;
              vector.erase(iter);
              return result;
            })
          .define_method("insert", [this](T& vector, Difference_T index, Parameter_T element) -> T&
            {
              index = normalizeIndex(vector.size(), index, true);
              auto iter = vector.begin() + index;
              vector.insert(iter, element);
              return vector;
            })
          .define_method("pop", [](T& vector) -> std::optional<Value_T>
            {
              if (vector.size() > 0)
              {
                Value_T result = vector.back();
                vector.pop_back();
                return result;
              }
              else
              {
                return std::nullopt;
              }
            })
          .define_method("push", [](T& vector, Parameter_T element) -> T&
            {
              vector.push_back(element);
              return vector;
            })
          .define_method("shrink_to_fit", &T::shrink_to_fit)
          .define_method("[]=", [this](T& vector, Difference_T index, Parameter_T element) -> Parameter_T
            {
              index = normalizeIndex(vector.size(), index, true);
              vector[index] = element;
              return element;
            });

        rb_define_alias(klass_, "push_back", "push");
        rb_define_alias(klass_, "<<", "push");
        rb_define_alias(klass_, "append", "push");
      }

      void define_enumerable()
      {
        // Add enumerable support
        klass_.template define_iterator<typename T::iterator(T::*)()>(&T::begin, &T::end);
      }

      void define_to_array()
      {
        // Add enumerable support
        klass_.define_method("to_a", [](T& vector)
        {
          VALUE result = rb_ary_new();
          std::for_each(vector.begin(), vector.end(), [&result](const Reference_T element)
          {
            VALUE value = detail::To_Ruby<Parameter_T>().convert(element);
            rb_ary_push(result, value);
          });

          return result;
        }, Return().setValue());
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<Value_T>)
        {
          klass_.define_method("to_s", [](const T& vector)
            {
              auto iter = vector.begin();
              auto finish = vector.size() > 1000 ? vector.begin() + 1000 : vector.end();

              std::stringstream stream;
              stream << "[";

              for (; iter != finish; iter++)
              {
                if (iter == vector.begin())
                {
                  stream << *iter;
                }
                else
                {
                  stream << ", " << *iter;
                }
              }

              stream << "]";
              return stream.str();
            });
        }
        else
        {
          klass_.define_method("to_s", [](const T& vector)
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
  Data_Type<std::vector<T>> define_vector(std::string klassName)
  {
    using Vector_T = std::vector<T>;
    using Data_Type_T = Data_Type<Vector_T>;

    if (klassName.empty())
    {
      std::string typeName = detail::typeName(typeid(Vector_T));
      klassName = detail::rubyClassName(typeName);
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<Vector_T>>(rb_mStd, id);
    stl::VectorHelper helper(result);
    return result;
  }


  namespace detail
  {
    template<typename T>
    struct Type<std::vector<T>>
    {
      static bool verify()
      {
        Type<intrinsic_type<T>>::verify();

        if (!Data_Type<std::vector<T>>::is_defined())
        {
          define_vector<T>();
        }

        return true;
      }
    };

    template<typename T>
    class From_Ruby<std::vector<T>>
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
            return Data_Type<std::vector<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_ARRAY:
            if constexpr (std::is_default_constructible_v<T>)
            {
              return Convertible::Cast;
            }
          default:
            return Convertible::None;
        }
      }

      std::vector<T> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped vector (hopefully!)
            return *detail::unwrap<std::vector<T>>(value, Data_Type<std::vector<T>>::ruby_data_type(), false);
          }
          case RUBY_T_ARRAY:
          {
            // If this an Ruby array and the vector type is copyable
            if constexpr (std::is_default_constructible_v<T>)
            {
              return Array(value).to_vector<T>();
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::vector");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<typename T>
    class From_Ruby<std::vector<T>&>
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
            return Data_Type<std::vector<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_ARRAY:
            if constexpr (std::is_default_constructible_v<T>)
            {
              return Convertible::Cast;
            }
          default:
            return Convertible::None;
        }
      }

      std::vector<T>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped vector (hopefully!)
            return *detail::unwrap<std::vector<T>>(value, Data_Type<std::vector<T>>::ruby_data_type(), false);
          }
          case RUBY_T_ARRAY:
          {
            // If this an Ruby array and the vector type is copyable
            if constexpr (std::is_default_constructible_v<T>)
            {
              this->converted_ = Array(value).to_vector<T>();
              return this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::vector");
          }
        }
      }

    private:
      Arg* arg_ = nullptr;
      std::vector<T> converted_;
    };

    template<typename T>
    class From_Ruby<std::vector<T>*>
    {
    public:
      Convertible is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            return Data_Type<std::vector<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_NIL:
            return Convertible::Exact;
            break;
          case RUBY_T_ARRAY:
            if constexpr (std::is_default_constructible_v<T>)
            {
              return Convertible::Cast;
            }
          default:
            return Convertible::None;
        }
      }

      std::vector<T>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
            // This is a wrapped vector (hopefully!)
            return detail::unwrap<std::vector<T>>(value, Data_Type<std::vector<T>>::ruby_data_type(), false);
          }
          case RUBY_T_ARRAY:
          {
            // If this a Ruby array and the vector type is copyable
            if constexpr (std::is_default_constructible_v<T>)
            {
              this->converted_ = Array(value).to_vector<T>();
              return &this->converted_;
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::vector");
          }
        }
      }

    private:
      std::vector<T> converted_;
    };
  }

  // Special handling for std::vector<bool>
  namespace detail
  {
    template<>
    class To_Ruby<std::vector<bool>::reference>
    {
    public:
      VALUE convert(const std::vector<bool>::reference& value)
      {
        return value ? Qtrue : Qfalse;
      }
    };
  }
}


#endif // Rice__stl__hpp_
