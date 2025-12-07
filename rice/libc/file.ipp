#include <exception>

RICE_BEGIN_NAMESPACE
  namespace libc
  {
    inline Class rb_cLibcFile;

    inline void define_libc_file()
    {
      Module rb_mLibc = RICE_DEFINE_MODULE_RICE_LIBC;
      rb_cLibcFile = define_class_under<FILE>(rb_mLibc, "File");
    }
  }
RICE_END_NAMESPACE

RICE_DETAIL_BEGIN_NAMESPACE
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
RICE_DETAIL_END_NAMESPACE
