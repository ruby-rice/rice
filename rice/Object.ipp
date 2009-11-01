#ifndef Rice__Object__ipp_
#define Rice__Object__ipp_

#include "protect.hpp"
#include "detail/ruby.hpp"

#include "detail/object_call.ipp"

template<typename T>
void Rice::Object::
iv_set(
    Identifier name,
    T const & value)
{
  protect(rb_ivar_set, *this, name.id(), to_ruby(value));
}

#endif // Rice__Object__ipp_

