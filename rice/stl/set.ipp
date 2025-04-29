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
