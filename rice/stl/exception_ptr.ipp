#include <exception>

namespace Rice::stl
{
  inline Data_Type<std::exception_ptr> define_exception_ptr()
  {
    Module rb_mStd = define_module("Std");
    return define_class_under<std::exception_ptr>(rb_mStd, "ExceptionPtr");
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::exception_ptr>
  {
    static bool verify()
    {
      if (!Data_Type<std::exception_ptr>::is_defined())
      {
        stl::define_exception_ptr();
      }

      return true;
    }
  };
}
