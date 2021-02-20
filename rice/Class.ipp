#ifndef Rice__Class__ipp_
#define Rice__Class__ipp_

#include "Exception.hpp"
#include "Data_Type_defn.hpp"

inline
Rice::Class::
Class(VALUE v)
  : Module(v)
{
  if (::rb_type(v) != T_CLASS)
  {
    throw Exception(
      rb_eTypeError,
      "Expected a Class but got a %s",
      rb_class2name(CLASS_OF(v))); // TODO: might raise an exception
  }
}

inline
Rice::Class& Rice::Class::
undef_creation_funcs()
{
  rb_undef_alloc_func(value());
  rb_undef_method(value(), "initialize");
  return *this;
}

inline
Rice::Class Rice::
define_class_under(
  Rice::Object module,
  char const* name,
  Rice::Object superclass)
{
  VALUE v = rb_define_class_under(module, name, superclass);
  return Class(v);
}

inline
Rice::Class Rice::
define_class(
  char const* name,
  Rice::Object superclass)
{
  VALUE v = rb_define_class(name, superclass);
  return Class(v);
}

inline
Rice::Class Rice::
anonymous_class()
{
  return Class(rb_class_new(rb_cObject));
}

#endif // Rice__Class__ipp_
