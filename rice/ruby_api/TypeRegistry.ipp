using namespace Rice;

extern "C"
void Init_Type_Registry()
{
  Module rb_mRice = define_module("Rice");

  Rice::define_class_under<detail::TypeRegistry>(rb_mRice, "TypeRegistry").
    define_method("klasses", &detail::TypeRegistry::klasses, Return().setValue());
}
