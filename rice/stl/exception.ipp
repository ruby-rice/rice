#include <exception>

// Libraries sometime inherit custom exception objects from std::exception,
// so define it for Ruby if necessary
namespace Rice
{
  namespace stl
  {
    inline Data_Type<std::exception> define_stl_exception()
    {
      Module rb_mRice = define_module("Rice");
      Module rb_mmap = define_module_under(rb_mRice, "Std");
      Data_Type<std::exception> rb_cStlException = define_class_under<std::exception>(rb_mmap, "Exception");
      rb_cStlException.
         define_constructor(Constructor<std::exception>()).
         define_method("what", &std::exception::what);
      return rb_cStlException;
    }
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
