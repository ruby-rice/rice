#include <exception>

RICE_STL_BEGIN_NAMESPACE
  inline Data_Type<std::exception_ptr> define_exception_ptr()
  {
    Module rb_mStd = RICE_DEFINE_MODULE_RICE_STL;
    return define_class_under<std::exception_ptr>(rb_mStd, "ExceptionPtr");
  }
RICE_STL_END_NAMESPACE

RICE_DETAIL_BEGIN_NAMESPACE
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
RICE_DETAIL_END_NAMESPACE
