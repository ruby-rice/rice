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
    // Not sure why this stuff can't be chained
    VALUE gotV = protect(rb_mod_name, rb_obj_class(value));
    char* got = StringValuePtr(gotV);
    VALUE exptV = protect(rb_mod_name, klass);
    char* expected = StringValuePtr(exptV);

    throw Exception(
        rb_eTypeError,
        "wrong argument type %s (expected %s)",
        got, expected
      );
  }                                 
}

