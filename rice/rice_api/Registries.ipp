using namespace Rice;

inline void Init_Registries()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<detail::Registries>(rb_mRice, "Registries").
    define_singleton_attr("instance", &detail::Registries::instance, AttrAccess::Read).
    define_attr("modules", &detail::Registries::modules, AttrAccess::Read).
    define_attr("natives", &detail::Registries::natives, AttrAccess::Read).
    define_attr("types", &detail::Registries::types, AttrAccess::Read);
}
