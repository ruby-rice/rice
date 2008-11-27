#include "Object.hpp"

inline Rice::Exception_Base::
Exception_Base(VALUE v)
  : Object(v)
{
}

inline Rice::Exception_Base::
~Exception_Base() throw()
{
}

