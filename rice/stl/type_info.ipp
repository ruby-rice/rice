#include <typeinfo>

namespace Rice::stl
{
  inline Data_Type<std::type_info> define_type_info()
  {
    Module rb_mRice = define_module("Rice");
    Module rb_mStd = define_module_under(rb_mRice, "Std");

    return define_class_under<std::type_info>(rb_mStd, "TypeInfo").
      define_method("hash_code", &std::type_info::hash_code).
      define_method("name", &std::type_info::name);
  }
}

namespace Rice::detail
{
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
}
