#include "Class.hpp"
#include "Exception.hpp"
#include "detail/creation_funcs.hpp"

Rice::Class::
Class()
  : Module_impl<Module, Class>()
{
}

Rice::Class::
Class(VALUE v)
  : Module_impl<Module, Class>(v)
{
  if(::rb_type(v) != T_CLASS)
  {
    throw Exception(
        rb_eTypeError,
        "Expected a Class but got a %s",
        rb_class2name(CLASS_OF(v))); // TODO: might raise an exception
  }
}

Rice::Class & Rice::Class::
undef_creation_funcs()
{
  detail::undef_creation_funcs(*this);
  return *this;
}

Rice::Class Rice::
define_class_under(
    Rice::Object module,
    char const * name,
    Rice::Object superclass)
{
  VALUE v = rb_define_class_under(module, name, superclass);
  return Class(v);
}

Rice::Class Rice::
define_class(
    char const * name,
    Rice::Object superclass)
{
  VALUE v = rb_define_class(name, superclass);
  return Class(v);
}

Rice::Class Rice::
anonymous_class(
    Rice::Object superclass)
{
  VALUE v = Class(rb_cClass).call("new");
  return Class(v);
}

