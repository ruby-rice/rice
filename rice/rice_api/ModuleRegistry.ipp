RICE_USE_NAMESPACE

extern "C"
void Init_Module_Registry()
{
  Module rb_mRice = RICE_DEFINE_MODULE_RICE;

  define_class_under<detail::ModuleRegistry>(rb_mRice, "ModuleRegistry").
    define_method("modules", &detail::ModuleRegistry::modules, Return().setValue());
}
