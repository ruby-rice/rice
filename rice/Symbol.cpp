#include "Symbol.hpp"

namespace
{
  VALUE check_type(Rice::Object value, int type)
  {
    rb_check_type(value, type);
    return Qnil;
  }
}

Rice::Symbol::
Symbol(VALUE v)
  : Object(v)
{
  protect(check_type, v, T_SYMBOL);
}

Rice::Symbol::
Symbol(Object v)
  : Object(v)
{
  protect(check_type, v, T_SYMBOL);
}

