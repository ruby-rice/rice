#ifndef Rice__ruby_Critical_Guard__ipp_
#define Rice__ruby_Critical_Guard__ipp_

#include "detail/ruby_version_code.hpp"

#if RICE__RUBY_VERSION_CODE < 190

#include "detail/ruby.hpp"
#include "detail/rubysig.hpp"

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

#endif

#endif // Rice__ruby_Critical_Guard__ipp_

