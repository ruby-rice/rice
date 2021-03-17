#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"
#include "../Data_Type.hpp"

#include <vector>

namespace Rice
{
  namespace stl
  {
    template<typename T>
    void define_vector_methods(Data_Type<T>& klass)
    {
      using Value_T = typename T::value_type;

      // Define the constructor
      klass.define_constructor(Constructor<T>());
  
      // Capacity
      klass.define_method("empty?", &T::empty)
        .define_method("capacity", &T::capacity)
        .define_method("max_size", &T::max_size)
        .define_method("reserve", &T::reserve)
        .define_method("size", &T::size);

      // Access elements
      klass.define_method("at", static_cast<Value_T& (T::*)(const size_t)>(&T::at))
        .define_method("include?", [](const T& self, const Value_T& value)
          {
            return std::find(self.begin(), self.end(), value) != self.end();
          })
        .define_method("last", static_cast<Value_T& (T::*)()>(&T::back))
        .define_method("first", static_cast<Value_T& (T::*)()>(&T::front))
        .define_method("[]", static_cast<const Value_T& (T::*)(const size_t) const>(&T::operator[]))
        .define_method("[]=", static_cast<Value_T& (T::*)(const size_t)>(&T::operator[]));

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
        .define_method("pop_back", &T::pop_back)
        .define_method("push", [](T& self, Value_T& value) -> T&
          {
            self.push_back(value);
            return self;
          })
        .define_method("resize", static_cast<void (T::*)(const size_t)>(&T::resize))
        .define_method("shrink_to_fit", &T::shrink_to_fit)
        .define_method("swap", &T::swap);
          
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

      // Last add in aliases
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