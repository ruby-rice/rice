RICE_USE_NAMESPACE

inline void Init_Arg()
{
  Module rb_mRice = RICE_DEFINE_MODULE_RICE;

  define_class_under<Arg>(rb_mRice, "Arg").
    define_attr("name", &Arg::name, AttrAccess::Read);
}
