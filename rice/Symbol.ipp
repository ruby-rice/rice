#ifndef Rice__Symbol__ipp_
#define Rice__Symbol__ipp_

#include "protect.hpp"
#include "detail/ruby.hpp"

inline Rice::Symbol::
Symbol(char const * s)
  : Object(ID2SYM(rb_intern(s)))
{
}

inline Rice::Symbol::
Symbol(std::string const & s)
  : Object(ID2SYM(rb_intern(s.c_str())))
{
}

inline Rice::Symbol::
Symbol(Identifier id)
  : Object(ID2SYM(id))
{
}

inline char const * Rice::Symbol::
c_str() const
{
  return to_id().c_str();
}

inline std::string Rice::Symbol::
str() const
{
  return to_id().str();
}

inline Rice::Identifier Rice::Symbol::
to_id() const
{
  return rb_to_id(value());
}

#endif // Rice__Symbol__ipp_

