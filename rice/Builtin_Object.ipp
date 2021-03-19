#ifndef Rice__Builtin_Object__ipp_
#define Rice__Builtin_Object__ipp_

#include "detail/Ruby_Function.hpp"
#include <algorithm>

namespace Rice
{

namespace detail
{
  inline VALUE check_type(Object value, int type)
  {
    detail::protect(rb_check_type, value.value(), type);
    return Qnil;
  }
}

template<int Builtin_Type>
inline Builtin_Object<Builtin_Type>::
Builtin_Object(Object value)
  : Object(value)
{
  detail::check_type(value, Builtin_Type);
}

template<int Builtin_Type>
inline RObject& Builtin_Object<Builtin_Type>::
operator*() const
{
  return *ROBJECT(this->value());
}

template<int Builtin_Type>
inline RObject* Builtin_Object<Builtin_Type>::
operator->() const
{
  return ROBJECT(this->value());
}

template<int Builtin_Type>
inline RObject* Builtin_Object<Builtin_Type>::
get() const
{
  return ROBJECT(this->value());
}

} // namespace Rice

#endif // Rice__Builtin_Object__ipp_

