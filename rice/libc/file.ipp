#include <exception>

// Libraries sometime inherit custom exception objects from std::exception,
// so define it for Ruby if necessary
namespace Rice::libc
{
  inline Class rb_cLibcFile;

  inline void define_libc_file()
  {
    Module rb_mRice = define_module("Rice");
    Module rb_mLibc = define_module_under(rb_mRice, "libc");
    rb_cLibcFile = define_class_under<FILE>(rb_mLibc, "File");
  }
}

namespace Rice::detail
{
  template<>
  struct Type<FILE>
  {
    static bool verify()
    {
      if (!Data_Type<FILE>::is_defined())
      {
        libc::define_libc_file();
      }

      return true;
    }
  };
}
