using namespace Rice;

extern "C"
void Init_Module_Registry()
{
  Module rb_mRice = define_module("Rice");

  Rice::define_class_under<detail::ModuleRegistry>(rb_mRice, "ModuleRegistry").
    define_method("modules", &detail::ModuleRegistry::modules, Return().setValue());
}
