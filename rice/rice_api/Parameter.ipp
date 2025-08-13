using namespace Rice;

inline void Init_Parameter()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<detail::ParameterAbstract>(rb_mRice, "Parameter").
    define_attr("arg", &detail::ParameterAbstract::arg).
    define_method("klass", &detail::ParameterAbstract::rubyKlass, Return().setValue()).
    define_method("cpp_klass", &detail::ParameterAbstract::cppTypeName).
    define_method("ruby_klass", &detail::ParameterAbstract::cppTypeName);
}
