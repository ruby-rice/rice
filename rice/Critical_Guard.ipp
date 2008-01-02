#ifndef Rice__ruby_Critical_Guard__ipp_
#define Rice__ruby_Critical_Guard__ipp_

#include "detail/ruby.hpp"
#include <rubysig.h>

inline Rice::Critical_Guard::
Critical_Guard()
{
  rb_thread_critical = 1;
}

inline Rice::Critical_Guard::
~Critical_Guard()
{
  rb_thread_critical = 0;
}

#endif // Rice__ruby_Critical_Guard__ipp_

