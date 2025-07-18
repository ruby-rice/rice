#include <rice/Arg.hpp>

using namespace Rice;

inline void Init_Arg()
{
  Module rb_mRice = define_module("Rice");

  define_class_under<Arg>(rb_mRice, "Arg").
    define_attr("name", &Arg::name, AttrAccess::Read);
}
