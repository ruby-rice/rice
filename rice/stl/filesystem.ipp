#include <filesystem>

RICE_BEGIN_NAMESPACE
  namespace stl
  {
    inline void define_filesystem_path()
    {
      Module rb_mStd = RICE_DEFINE_MODULE_RICE_STL;
      Module rb_mFileSystem = define_module_under(rb_mStd, "Filesystem");

      define_class_under<std::filesystem::path>(rb_mFileSystem, "Path").
        define_constructor(Constructor<std::filesystem::path>()).
        define_constructor(Constructor<std::filesystem::path, std::string>());
    }
  }
RICE_END_NAMESPACE

RICE_DETAIL_BEGIN_NAMESPACE
  template<>
  struct Type<std::filesystem::path>
  {
    static bool verify()
    {
      if (!Data_Type<std::filesystem::path>::is_defined())
      {
        RICE_PREPEND_NAMESPACE(stl)::define_filesystem_path();
      }

      return true;
    }
  };
RICE_DETAIL_END_NAMESPACE
