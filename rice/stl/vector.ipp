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
          klass_.template define_method<Value_T*(T::*)()>("data", &T::data);
        }

        rb_define_alias(klass_, "at", "[]");
      }

      // Methods that require Value_T to support operator==
      void define_comparable_methods()
      {
        if constexpr (detail::is_comparable_v<T>)
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
      From_Ruby() = default;

      explicit From_Ruby(Arg* arg) : arg_(arg)
      {
      }

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
      Arg* arg_;
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
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo)
      {
      }

      VALUE convert(const std::vector<bool>::reference& value)
      {
        return value ? Qtrue : Qfalse;
      }
    };
  }
}
