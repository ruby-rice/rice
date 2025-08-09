#include <exception>

namespace Rice::Libc
{
  inline Class rb_cLibcFile;

  inline void define_libc_file()
  {
    Module rb_mLibc = define_module("Libc");
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
        Libc::define_libc_file();
      }

      return true;
    }
  };
}
