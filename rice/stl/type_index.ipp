#include <typeindex>

RICE_STL_BEGIN_NAMESPACE
  inline Data_Type<std::type_index> define_type_index()
  {
    Module rb_mStd = RICE_DEFINE_MODULE_RICE_STL;
    return define_class_under<std::type_index>(rb_mStd, "TypeIndex").
      define_constructor(Constructor<std::type_index, const std::type_info&>()).
      define_method("hash_code", &std::type_index::hash_code).
      define_method("name", &std::type_index::name);
  }
RICE_STL_END_NAMESPACE

RICE_DETAIL_BEGIN_NAMESPACE
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
RICE_DETAIL_END_NAMESPACE
