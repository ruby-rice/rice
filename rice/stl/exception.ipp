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
