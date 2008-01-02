#include "check_ruby_type.hpp"
#include "../Exception.hpp"

void Rice::detail::
check_ruby_type(
    VALUE value,
    VALUE klass,
    bool include_super
    )
{ 
  if( !rb_obj_is_kind_of(value, klass) ||
     (!include_super && rb_obj_class(value) != klass))
  {
    throw Exception(
        rb_eTypeError,
        "wrong argument type %s (expected %s)",
        STR2CSTR(protect(rb_mod_name, rb_obj_class(value))),
        STR2CSTR(protect(rb_mod_name, klass)));
  }                                 
}

