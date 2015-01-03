#ifndef Rice__Builtin_Object__ipp_
#define Rice__Builtin_Object__ipp_

#include "Object.hpp"
#include "protect.hpp"
#include "detail/check_ruby_type.hpp"

#include <algorithm>

namespace Rice
{

namespace detail
{
  inline VALUE check_type(Object value, int type)
  {
    rb_check_type(value, type);
    return Qnil;
  }
}

template<int Builtin_Type>
inline Builtin_Object<Builtin_Type>::
Builtin_Object(Object value)
  : Object(value)
  , obj_((RObject*)(value.value()))
{
  protect(detail::check_type, value, Builtin_Type);
}

template<int Builtin_Type>
inline Builtin_Object<Builtin_Type>::
Builtin_Object(Builtin_Object<Builtin_Type> const & other)
  : Object(other.value())
  , obj_(other.obj_)
{
}

template<int Builtin_Type>
inline void Builtin_Object<Builtin_Type>::
swap(Builtin_Object<Builtin_Type> & ref)
{
  std::swap(obj_, ref.obj_);
  Object::swap(ref);
}

} // namespace Rice

#endif // Rice__Builtin_Object__ipp_

