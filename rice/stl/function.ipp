#include <functional>

namespace Rice::stl
{
  template<typename T>
  class FunctionHelper
  {
    using Function_T = T;

  public:
    FunctionHelper(Data_Type<T> klass) : klass_(klass)
    {
      this->define_constructors();
      this->define_methods();
    }

  private:
    void define_constructors()
    {
      // Default constructor
      klass_.define_constructor(Constructor<Function_T>());

      // Constructor from Ruby callable
      klass_.define_method("initialize", [](VALUE self, VALUE callable) -> void
      {
        // Create std::function that wraps the Ruby callable
        Function_T* data = new Function_T([callable](auto... args)
        {
          Object result = Object(callable).call("call", args...);

          using Return_T = typename Function_T::result_type;
          if constexpr (!std::is_void_v<Return_T>)
          {
            return detail::From_Ruby<std::remove_cv_t<Return_T>>().convert(result);
          }
        });

        // Wrap the function
        detail::wrapConstructed<T>(self, Data_Type<T>::ruby_data_type(), data);
      }, Arg("callable").setValue().keepAlive());
    }

    void define_methods()
    {
      klass_.define_method("call", &Function_T::operator());
      klass_.define_method("callable?", [](const Function_T& func) -> bool
      {
        return static_cast<bool>(func);
      });
    }

    Data_Type<T> klass_;
  };
}

namespace Rice
{
  template<typename Signature_T>
  Data_Type<std::function<Signature_T>> define_stl_function(std::string klassName)
  {
    using Function_T = std::function<Signature_T>;
    using Data_Type_T = Data_Type<Function_T>;

    if (klassName.empty())
    {
      detail::TypeDetail<Function_T> typeDetail;
      klassName = typeDetail.rubyName();
    }

    Module rb_mStd = define_module("Std");
    if (Data_Type_T::check_defined(klassName, rb_mStd))
    {
      return Data_Type_T();
    }

    Identifier id(klassName);
    Data_Type_T result = define_class_under<detail::intrinsic_type<Function_T>>(rb_mStd, id);

    stl::FunctionHelper helper(result);

    return result;
  }
}

namespace Rice::detail
{
  template<typename Signature_T>
  struct Type<std::function<Signature_T>>
  {
    static bool verify()
    {
      define_stl_function<Signature_T>();
      return true;
    }
  };
}
