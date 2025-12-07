#include <typeinfo>

RICE_STL_BEGIN_NAMESPACE
  inline Data_Type<std::type_info> define_type_info()
  {
    Module rb_mStd = RICE_DEFINE_MODULE_RICE_STL;
    return define_class_under<std::type_info>(rb_mStd, "TypeInfo").
      define_method("hash_code", &std::type_info::hash_code).
      define_method("name", &std::type_info::name);
  }
RICE_STL_END_NAMESPACE

RICE_DETAIL_BEGIN_NAMESPACE
  template<>
  struct Type<std::type_info>
  {
    static inline bool verify()
    {
      if (!detail::Registries::instance.types.isDefined<std::type_info>())
      {
        stl::define_type_info();
      }

      return true;
    }
  };
RICE_DETAIL_END_NAMESPACE
