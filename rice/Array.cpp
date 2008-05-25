#include "Array.hpp"

Rice::Array Rice::Array::
concat(Array a)
{
  return protect(rb_ary_concat, *this, a);
}

Rice::Array Rice::Array::
operator+=(Array rhs)
{
  return concat(rhs);
}


