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
      // Define the constructor
      klass.define_constructor(Constructor<T>());
  
      // Capacity
      klass.define_method("empty?", &T::empty)
        .define_method("capacity", &T::capacity)
        .define_method("max_size", &T::max_size)
        .define_method("reserve", &T::reserve)
        .define_method("size", &T::size);

      // Access
      klass.define_method("back", static_cast<T::value_type&(T::*)()>(&T::back))
        .define_method("front", &T::front);

       // .define_method("shrink_to_fit", &T::size);

    // Define basic methods
        //klass.define_method("clear", &T::clear)
          //        .define_method("first", &T::front)
                  //.define_method("last", &T::back)
          
      // Add enumerable support
      klass.include_module(rb_mEnumerable)
      .define_method("each", [](const T& self)
        {
          for (const T::value_type& item : self)
          {
            rb_yield(detail::To_Ruby<T::value_type>::convert(item));
          }
          return self;
        });

      // Last add in aliases
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