#include <exception>

// Libraries sometime inherit custom exception objects from std::exception,
// so define it for Ruby if necessary
namespace Rice::stl
{
  inline void define_stl_exceptions()
  {
    Module rb_mStd = define_module("Std");

    define_class_under<std::exception>(rb_mStd, "Exception", rb_eStandardError).
      define_constructor(Constructor<std::exception>()).
      define_method("message", &std::exception::what);

    define_class_under<std::runtime_error>(rb_mStd, "RuntimeError", rb_eRuntimeError).
      define_constructor(Constructor<std::runtime_error, const char*>()).
      define_method("message", &std::runtime_error::what);
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::exception>
  {
    static bool verify()
    {
      Rice::stl::define_stl_exceptions();
      return true;
    }
  };

  template<>
  struct Type<std::runtime_error>
  {
    static bool verify()
    {
      Rice::stl::define_stl_exceptions();
      return true;
    }
  };
}
