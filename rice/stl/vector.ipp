#include "../detail/rice_traits.hpp"
#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"
#include "../Data_Type.hpp"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    class VectorHelper
    {
      using Value_T = typename T::value_type;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;

    public:
      VectorHelper(Data_Type<T> klass) : klass_(klass)
      {
        this->define_constructor();
        this->define_copyable_methods();
        this->define_constructable_methods();
        this->define_capacity_methods();
        this->define_access_methods();
        this->define_comparable_methods();
        this->define_modify_methods();
        this->define_to_s();
        this->define_enumerable();
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
              throw std::runtime_error("Cannot copy vectors with non-copy constructible types");
              return self;
            });
        }
      }

      void define_constructable_methods()
      {
        if constexpr (std::is_default_constructible_v<Value_T>)
        {
          klass_.define_method("resize", static_cast<void (T::*)(const size_t)>(&T::resize));
        }
        else
        {
          klass_.define_method("resize", [](const T& self, Size_T newSize)
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
        klass_.define_method("first", [](const T& self) -> std::optional<Value_T>
          {
            if (self.size() > 0)
            {
              return self.front();
            }
            else
            {
              return std::nullopt;
            }
          })
          .define_method("last", [](const T& self) -> std::optional<Value_T>
            {
              if (self.size() > 0)
              {
                return self.back();
              }
              else
              {
                return std::nullopt;
              }
            })
            .define_method("[]", [this](const T& self, Difference_T index) -> std::optional<Value_T>
              {
                index = normalizeIndex(self.size(), index);
                if (index < 0 || index >= (Difference_T)self.size())
                {
                  return std::nullopt;
                }
                else
                {
                  return self[index];
                }
              });

            rb_define_alias(klass_, "at", "[]");
      }

      // Methods that require Value_T to support operator==
      void define_comparable_methods()
      {
        if constexpr (detail::is_comparable_v<Value_T>)
        {
          klass_.define_method("delete", [](T& self, Value_T& element) -> std::optional<Value_T>
            {
              auto iter = std::find(self.begin(), self.end(), element);
              if (iter == self.end())
              {
                return std::nullopt;
              }
              else
              {
                Value_T result = *iter;
                self.erase(iter);
                return result;
              }
            })
          .define_method("include?", [](T& self, Value_T& element)
            {
              return std::find(self.begin(), self.end(), element) != self.end();
            })
          .define_method("index", [](T& self, Value_T& element) -> std::optional<Difference_T>
            {
              auto iter = std::find(self.begin(), self.end(), element);
              if (iter == self.end())
              {
                return std::nullopt;
              }
              else
              {
                return iter - self.begin();
              }
            });
        }
        else
        {
          klass_.define_method("delete", [](T& self, Value_T& element) -> std::optional<Value_T>
            {
              return std::nullopt;
            })
          .define_method("include?", [](const T& self, Value_T& element)
            {
              return false;
            })
          .define_method("index", [](const T& self, Value_T& element) -> std::optional<Difference_T>
            {
              return std::nullopt;
            });
        }
      }

      void define_modify_methods()
      {
        klass_.define_method("clear", &T::clear)
          .define_method("delete_at", [](T& self, const size_t& pos)
            {
              auto iter = self.begin() + pos;
              Value_T result = *iter;
              self.erase(iter);
              return result;
            })
          .define_method("insert", [this](T& self, Difference_T index, Value_T& element) -> T&
            {
              index = normalizeIndex(self.size(), index, true);
              auto iter = self.begin() + index;
              self.insert(iter, element);
              return self;
            })
          .define_method("pop", [](T& self) -> std::optional<Value_T>
            {
              if (self.size() > 0)
              {
                Value_T result = self.back();
                self.pop_back();
                return result;
              }
              else
              {
                return std::nullopt;
              }
            })
          .define_method("push", [](T& self, Value_T& element) -> T&
            {
              self.push_back(element);
              return self;
            })
          .define_method("shrink_to_fit", &T::shrink_to_fit)
          .define_method("[]=", [this](T& self, Difference_T index, Value_T& element) -> Value_T&
            {
              index = normalizeIndex(self.size(), index, true);
              self[index] = element;
              return element;
            });

            rb_define_alias(klass_, "<<", "push");
            rb_define_alias(klass_, "append", "push");
      }

      void define_enumerable()
      {
        // Add enumerable support
        klass_.include_module(rb_mEnumerable)
          .define_method("each", [](T& self) -> const T&
            {
              for (Value_T& item : self)
              {
                VALUE element = detail::To_Ruby<Value_T>::convert(item, false);
                rb_yield(element);
              }
              return self;
            });
      }

      void define_to_s()
      {
        if constexpr (detail::is_ostreamable_v<Value_T>)
        {
          klass_.define_method("to_s", [](const T& self)
            {
              auto iter = self.begin();
              auto finish = self.size() > 1000 ? self.begin() + 1000 : self.end();

              std::stringstream stream;
              stream << "[";

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

              stream << "]";
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
  Data_Type<T> define_vector_under(Object module, std::string name)
  {
    if (detail::TypeRegistry::isDefined<T>())
    {
      return Data_Type<T>(Data_Type<T>());
    }

    Data_Type<T> result = define_class_under<detail::intrinsic_type<T>>(module, name.c_str());
    stl::VectorHelper helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_vector(std::string name)
  {
    if (detail::TypeRegistry::isDefined<T>())
    {
      return Data_Type<T>(Data_Type<T>());
    }

    Data_Type<T> result = define_class<detail::intrinsic_type<T>>(name.c_str());
    stl::VectorHelper<T> helper(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_vector_auto()
  {
    std::string klassName = detail::makeClassName(typeid(T));
    Module rb_mRice = define_module("Rice");
    Module rb_mVector = define_module_under(rb_mRice, "Std");
    return define_vector_under<T>(rb_mVector, klassName);
  }
   
  namespace detail
  {
    template<typename T>
    struct Type<std::vector<T>>
    {
      static bool verify()
      {
        Type<T>::verify();

        if (!detail::TypeRegistry::isDefined<std::vector<T>>())
        {
          define_vector_auto<std::vector<T>>();
        }

        return true;
      }
    };

    template<typename T>
    std::vector<T> vectorFromArray(VALUE value)
    {
      size_t length = protect(rb_array_len, value);
      std::vector<T> result(length);

      for (long i = 0; i < length; i++)
      {
        VALUE element = protect(rb_ary_entry, value, i);
        result[i] = From_Ruby<T>().convert(element);
      }

      return result;
    }

    template<typename T>
    class From_Ruby<std::vector<T>>
    {
    public:
      std::vector<T> convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case T_DATA:
          {
            // This is a wrapped vector (hopefully!)
            return *Data_Object<std::vector<T>>::from_ruby(value);
          }
          case T_ARRAY:
          {
            // If this an Ruby array and the vector type is copyable
            if constexpr (std::is_default_constructible_v<T>)
            {
              return vectorFromArray<T>(value);
            }
          }
          default:
          {
            throw Exception(rb_eTypeError, "wrong argument type %s (expected % s)",
              detail::protect(rb_obj_classname, value), "std::vector");
          }
        }
      }
    };

    template<typename T>
    class From_Ruby<std::vector<T>&>
    {
    public:
      std::vector<T>& convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case T_DATA:
          {
            // This is a wrapped vector (hopefully!)
            return *Data_Object<std::vector<T>>::from_ruby(value);
          }
          case T_ARRAY:
          {
            // If this an Ruby array and the vector type is copyable
            if constexpr (std::is_default_constructible_v<T>)
            {
              this->converted_ = vectorFromArray<T>(value);
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
      std::vector<T> converted_;
    };

    template<typename T>
    class From_Ruby<std::vector<T>*>
    {
    public:
      std::vector<T>* convert(VALUE value)
      {
        switch (rb_type(value))
        {
          case T_DATA:
          {
            // This is a wrapped vector (hopefully!)
            return Data_Object<std::vector<T>>::from_ruby(value);
          }
          case T_ARRAY:
          {
            // If this an Ruby array and the vector type is copyable
            if constexpr (std::is_default_constructible_v<T>)
            {
              this->converted_ = vectorFromArray<T>(value);
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
}