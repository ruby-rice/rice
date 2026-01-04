#include <filesystem>

namespace Rice
{
  namespace stl
  {
    inline void define_filesystem_path()
    {
      Module rb_mStd = define_module("Std");
      Module rb_mFileSystem = define_module_under(rb_mStd, "Filesystem");

      define_class_under<std::filesystem::path>(rb_mFileSystem, "Path").
        define_constructor(Constructor<std::filesystem::path>()).
        define_constructor(Constructor<std::filesystem::path, std::string>(), Arg("source"));
    }
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::filesystem::path>
  {
    static bool verify()
    {
      if (!Data_Type<std::filesystem::path>::is_defined())
      {
        Rice::stl::define_filesystem_path();
      }

      return true;
    }
  };
}
