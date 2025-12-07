RICE_USE_NAMESPACE

inline void Init_Parameter()
{
  Module rb_mRice = RICE_DEFINE_MODULE_RICE;

  define_class_under<detail::ParameterAbstract>(rb_mRice, "Parameter").
    define_method("arg", &detail::ParameterAbstract::arg).
    define_method("klass", &detail::ParameterAbstract::klass, Return().setValue()).
    define_method("cpp_klass", &detail::ParameterAbstract::cppTypeName);
}
