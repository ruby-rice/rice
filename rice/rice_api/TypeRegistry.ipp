RICE_USE_NAMESPACE

extern "C"
void Init_Type_Registry()
{
  Module rb_mRice = RICE_DEFINE_MODULE_RICE;

  define_class_under<detail::TypeRegistry>(rb_mRice, "TypeRegistry").
    define_method("klasses", &detail::TypeRegistry::klasses, Return().setValue());
}
