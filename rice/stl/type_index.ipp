#include <typeindex>

namespace Rice::stl
{
  inline Data_Type<std::type_index> define_type_index()
  {
    Module rb_mRice = define_module("Rice");
    Module rb_mStd = define_module_under(rb_mRice, "Std");

    return define_class_under<std::type_index>(rb_mStd, "TypeIndex").
      define_constructor(Constructor<std::type_index, const std::type_info&>()).
      define_method("hash_code", &std::type_index::hash_code).
      define_method("name", &std::type_index::name);
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::type_index>
  {
    static bool verify()
    {
      if (!detail::Registries::instance.types.isDefined<std::type_index>())
      {
        stl::define_type_index();
      }

      return true;
    }
  };
}
