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
              .define_constructor(Constructor<T, const T&>(), Arg("other"));
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
          }, Arg("key"))
          .define_method("count", [](T& self, const Key_T element) -> Size_T
          {
            return self.count(element);
          }, Arg("key"));
      }

      void define_modify_methods()
      {
        klass_
          .define_method("clear", &T::clear)
          .define_method("delete", [](T& self, const Key_T key) -> T&
          {
            self.erase(key);
            return self;
          }, Arg("key"))
          .define_method("insert", [](T& self, Parameter_T value) -> T&
          {
            self.insert(value);
            return self;
          }, Arg("value").keepAlive())
          .define_method("merge", [](VALUE self, VALUE source)
          {
            T* selfPtr = detail::unwrap<T>(self, Data_Type<T>::ruby_data_type(), false);
            T* sourcePtr = detail::unwrap<T>(source, Data_Type<T>::ruby_data_type(), false);
            selfPtr->merge(*sourcePtr);

            // Merge moves elements from source to self, so copy keepAlive references.
            // This is conservative — duplicate elements that remain in source will also
            // have their keepAlive references copied, keeping them alive longer than
            // strictly necessary. This is safe (better than premature GC).
            detail::WrapperBase* selfWrapper = detail::getWrapper(self);
            detail::WrapperBase* sourceWrapper = detail::getWrapper(source);
            selfWrapper->setKeepAlive(sourceWrapper->getKeepAlive());

            return self;
          }, Arg("self").setValue(), Arg("source").setValue());

        rb_define_alias(klass_, "erase", "delete");
      }

      void define_operators()
      {
        klass_
          .define_method("<<", [](T& self, Parameter_T value) -> T&
          {
            self.insert(value);
            return self;
          }, Arg("value").keepAlive())
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
          }, Arg("other"))
          .define_method("&", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_intersection(self.begin(), self.end(),
                                  other.begin(), other.end(),
                                  std::inserter(result, result.begin()));

            return result;
          }, Arg("other"))
          .define_method("|", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_union(self.begin(), self.end(),
                           other.begin(), other.end(),
                           std::inserter(result, result.begin()));

            return result;
          }, Arg("other"))
          .define_method("-", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_difference(self.begin(), self.end(),
                                other.begin(), other.end(),
                                std::inserter(result, result.begin()));

            return result;
          }, Arg("other"))
          .define_method("^", [](const T& self, const T& other) -> T
          {
            T result;
            std::set_symmetric_difference(self.begin(), self.end(),
                                          other.begin(), other.end(),
                                          std::inserter(result, result.begin()));

            return result;
          }, Arg("other"))
          .define_method("<", [](const T& self, const T& other) -> bool
          {
            return std::includes(other.begin(), other.end(),
                                 self.begin(), self.end());
          }, Arg("other"))
          .define_method(">", [](const T& self, const T& other) -> bool
          {
            return std::includes(self.begin(), self.end(),
                                 other.begin(), other.end());
          }, Arg("other"));
        
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
          for (auto element: self)
          {
            array.push(element, false);
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
            detail::TypeDetail<T> typeDetail;
            stream << "<" << typeDetail.rubyName() << ":";
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

  template<typename T>
  Data_Type<std::set<T>> define_set(std::string klassName)
  {
    using Set_T = std::set<T>;
    using Data_Type_T = Data_Type<Set_T>;

    if (klassName.empty())
    {
      detail::TypeDetail<Set_T> typeDetail;
      klassName = typeDetail.rubyName();
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
      
      using Proc_T = decltype(block);
      using NativeProc_T = NativeProc<Proc_T>;
      std::unique_ptr<NativeProc_T> proc(NativeProc_T::define(std::forward<Proc_T>(block)));

      detail::protect<Function_T>(rb_block_call, rubySet, identifier.id(), 0, nullptr, NativeProc_T::resolve, (VALUE)proc.get());
      
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

      double is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
          {
          #if RUBY_API_VERSION_MAJOR >= 4
            if (detail::protect(rb_obj_is_instance_of, value, rb_cSet))
            {
              return Convertible::Exact;
            }
          #endif
            return Data_Type<std::set<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          }
          #if RUBY_API_VERSION_MAJOR < 4
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return Convertible::Exact;
            }
          }
          #endif
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
            #if RUBY_API_VERSION_MAJOR >= 4
            if (detail::protect(rb_obj_is_instance_of, value, rb_cSet))
            {
              return toSet<T>(value);
            }
            #endif

            if (Data_Type<std::set<T>>::is_descendant(value))
            {
              return *detail::unwrap<std::set<T>>(value, Data_Type<std::set<T>>::ruby_data_type(), false);
            }
          }
          #if RUBY_API_VERSION_MAJOR < 4
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return toSet<T>(value);
            }
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
          #endif
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
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

      double is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            #if RUBY_API_VERSION_MAJOR >= 4
            if (detail::protect(rb_obj_is_instance_of, value, rb_cSet))
            {
              return Convertible::Exact;
            }
            #endif
            return Data_Type<std::set<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          #if RUBY_API_VERSION_MAJOR < 4
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return Convertible::Exact;
            }
          }
          #endif
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
            #if RUBY_API_VERSION_MAJOR >= 4
            if (detail::protect(rb_obj_is_instance_of, value, rb_cSet))
            {
              // If this an Ruby array and the vector type is copyable
              if constexpr (std::is_default_constructible_v<T>)
              {
                this->converted_ = toSet<T>(value);
                return this->converted_;
              }
            }
            #endif

            if (Data_Type<std::set<T>>::is_descendant(value))
            {
              return *detail::unwrap<std::set<T>>(value, Data_Type<std::set<T>>::ruby_data_type(), false);
            }
          }
          #if RUBY_API_VERSION_MAJOR < 4
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
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
          #endif
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
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
      From_Ruby() = default;

      explicit From_Ruby(Arg* arg) : arg_(arg)
      {
      }

      double is_convertible(VALUE value)
      {
        switch (rb_type(value))
        {
          case RUBY_T_DATA:
            #if RUBY_API_VERSION_MAJOR >= 4
            if (detail::protect(rb_obj_is_instance_of, value, rb_cSet))
            {
              return Convertible::Exact;
            }
            #endif
            return Data_Type<std::set<T>>::is_descendant(value) ? Convertible::Exact : Convertible::None;
            break;
          case RUBY_T_NIL:
            return Convertible::Exact;
            break;
          #if RUBY_API_VERSION_MAJOR < 4
          case RUBY_T_OBJECT:
          {
            Object object(value);
            if (object.class_name().str() == setName)
            {
              return Convertible::Exact;
            }
          }
          #endif
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
            #if RUBY_API_VERSION_MAJOR >= 4
            if (detail::protect(rb_obj_is_instance_of, value, rb_cSet))
            {
              // If this an Ruby array and the vector type is copyable
              if constexpr (std::is_default_constructible_v<T>)
              {
                this->converted_ = toSet<T>(value);
                return &this->converted_;
              }
            }
            #endif

            if (Data_Type<std::set<T>>::is_descendant(value))
            {
              return detail::unwrap<std::set<T>>(value, Data_Type<std::set<T>>::ruby_data_type(), false);
            }
          }
          #if RUBY_API_VERSION_MAJOR < 4
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
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
          #endif
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
              detail::protect(rb_obj_classname, value), "std::set");
          }
        }
      }

    private:
      Arg* arg_;
      std::set<T> converted_;
    };
  }
}
