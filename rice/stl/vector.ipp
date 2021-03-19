#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"
#include "../Data_Type.hpp"

#include <sstream>
#include <vector>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    void define_vector_methods(Data_Type<T>& klass)
    {
      using Value_T = typename T::value_type;
      using Size_T = typename T::size_type;
      using Difference_T = typename T::difference_type;

      // Helper lambda to figure out indices
      auto normalizeIndex = [](Size_T size, Difference_T index, bool enforceBounds = false)
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

      // Define the constructor
      klass.define_constructor(Constructor<T>());
  
      // Capacity
      klass.define_method("empty?", &T::empty)
        .define_method("capacity", &T::capacity)
        .define_method("max_size", &T::max_size)
        .define_method("reserve", &T::reserve)
        .define_method("size", &T::size);

      // Access elements
      klass.define_method("include?", [](const T& self, const Value_T& value)
          {
            return std::find(self.begin(), self.end(), value) != self.end();
          })
        .define_method("first", [](T& self) -> std::optional<Value_T>
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
        .define_method("last", [](T& self) -> std::optional<Value_T>
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
        .define_method("[]", [&normalizeIndex](const T& self, Difference_T index) -> std::optional<Value_T>
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

      // Modify
      klass.define_method("clear", &T::clear)
        .define_method("delete", [](T& self, Value_T& value)
          {
            // Move elements we want to delete to the end
            auto iter = std::remove(self.begin(), self.end(), value);
            Value_T result = self.back();
            self.erase(iter, self.end());
            return result;
          })
        .define_method("delete_at", [](T& self, size_t& pos)
          {
            auto iter = self.begin() + pos;
            Value_T result = *iter;
            self.erase(iter);
            return result;
          })
        //.define_method("insert", static_cast<void (T::*)(const Value_T&)>(&T::insert))
        .define_method("insert", [&normalizeIndex](T& self, Difference_T index, Value_T& value) -> T&
          {
            index = normalizeIndex(self.size(), index, true);
            auto iter = self.begin() + index;
            self.insert(iter, value);
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
        .define_method("push", [](T& self, Value_T& value) -> T&
          {
            self.push_back(value);
            return self;
          })
        .define_method("resize", static_cast<void (T::*)(const size_t)>(&T::resize))
        .define_method("shrink_to_fit", &T::shrink_to_fit)
        .define_method("swap", &T::swap)
        .define_method("[]=", [&normalizeIndex](T& self, Difference_T index, Value_T& value) -> Value_T&
          {
            index = normalizeIndex(self.size(), index);
            self[index] = value;
            return value;
          });

      // Add enumerable support
      klass.include_module(rb_mEnumerable)
      .define_method("each", [](const T& self)
        {
          for (const Value_T& item : self)
          {
            rb_yield(detail::To_Ruby<Value_T>::convert(item));
          }
          return self;
        });

      // to_s support
      klass.define_method("to_s", [](const T& self)
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

      // Last add in aliases
      rb_define_alias(klass, "at", "[]");
      rb_define_alias(klass, "<<", "push");
      rb_define_alias(klass, "append", "push");
      rb_define_alias(klass, "count", "size");
      rb_define_alias(klass, "length", "size");
    }
  }

  template<typename T>
  Data_Type<T> define_vector_under(Object module, std::string name)
  {
    Data_Type<T> result = define_class_under<T>(module, name.c_str());
    stl::define_vector_methods(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_vector(std::string name)
  {
    Data_Type<T> result = define_class<T>(name.c_str());
    stl::define_vector_methods(result);
    return result;
  }

  template<typename T>
  Data_Type<T> define_vector_auto()
  {
    std::string name = detail::typeName(typeid(T::value_type));
    Module rb_mRice = define_module("Rice");
    Module rb_mVector = define_module_under(rb_mRice, "Vector");
    return define_vector_under<T>(rb_mVector, name);
  }
   
  namespace detail
  {
    template<typename T>
    struct Type<std::vector<T>>
    {
      constexpr static void verify()
      {
        if (!Data_Type<std::vector<T>>::isDefined)
        {
          define_vector_auto<std::vector<T>>();
        }
      }
    };

    /*template <typename T>
    struct To_Ruby<std::vector<T>>
    {
      static VALUE convert(const std::vector<T>& data, bool takeOwnership = false)
      {
        using Vector_T = Rice::stl::Vector<std::vector<T>>;
        return detail::wrap(Vector_T::klass(), Vector_T::rb_type(), data, takeOwnership);
      }
    };

    template <typename T>
    struct From_Ruby<std::vector<T>>
    {
      static std::vector<T> convert(VALUE value)
      {
        using Vector_T = Rice::stl::Vector<std::vector<T>>;
        return detail::unwrap<std::vector<T>>(value, Vector_T::rb_type());
      }
    };

    template <typename T>
    struct From_Ruby<std::vector<T>&>
    {
      static std::vector<T>& convert(VALUE value)
      {
        using Vector_T = Rice::stl::Vector<std::vector<T>>;
        return *detail::unwrap<std::vector<T>>(value, Vector_T::rb_type());
      }
    };*/
  }
}