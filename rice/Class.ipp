#ifndef Rice__Class__ipp_
#define Rice__Class__ipp_

#include "Exception.hpp"
#include "Data_Type_defn.hpp"

namespace Rice
{
  inline Class::Class(VALUE v) : Module(v)
  {
    if (::rb_type(v) != T_CLASS)
    {
      // TODO: might raise an exception
      throw Exception(rb_eTypeError, "Expected a Class but got a %s", rb_class2name(CLASS_OF(v)));
    }
  }

  inline Class& Class::undef_creation_funcs()
  {
    rb_undef_alloc_func(value());
    rb_undef_method(value(), "initialize");
    return *this;
  }

  inline Class define_class_under(Object module, char const* name, Object superclass)
  {
    VALUE v = rb_define_class_under(module, name, superclass);
    return Class(v);
  }

  inline Class define_class(char const* name, Object superclass)
  {
    VALUE v = rb_define_class(name, superclass);
    return Class(v);
  }

  inline Class anonymous_class()
  {
    return Class(rb_class_new(rb_cObject));
  }

  template<>
  struct detail::From_Ruby<Class>
  {
    static Class convert(VALUE value)
    {
      return Class(value);
    }
  };
}
#endif // Rice__Class__ipp_
