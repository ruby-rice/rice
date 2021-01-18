#ifndef Rice__Object__ipp_
#define Rice__Object__ipp_

#include "protect.hpp"

namespace Rice
{
  inline const Object Nil(Qnil);
  inline const Object True(Qtrue);
  inline const Object False(Qfalse);
  inline const Object Undef(Qundef);
}

template<typename ...ArgT>
inline Rice::Object Rice::Object::
call(Identifier id, ArgT... args) const
{
  auto asList = this->convert_args<ArgT...>(args...);
  return protect(rb_funcall2, value(), id, (int)asList.size(), asList.data());
}

template<typename ...ArgT>
std::vector<VALUE> Rice::Object::convert_args(ArgT&... args) const {
  return std::vector<VALUE>{ detail::To_Ruby<ArgT>::convert(args)... };
}

template<typename T>
inline void Rice::Object::
iv_set(
    Identifier name,
    T const & value)
{
  protect(rb_ivar_set, *this, name.id(), detail::To_Ruby<T>::convert(value));
}

inline int Rice::Object::
compare(Object const& other) const
{
  Object result = call("<=>", other);
  return Rice::detail::From_Ruby<int>::convert(result);
}

inline void Rice::Object::
freeze()
{
  protect(rb_obj_freeze, value());
}

inline bool Rice::Object::
is_frozen() const
{
  return bool(OBJ_FROZEN(value()));
}

inline void Rice::Object::
swap(Rice::Object& other)
{
  std::swap(value_, other.value_);
}

inline int Rice::Object::
rb_type() const
{
  return ::rb_type(*this);
}

inline bool Rice::Object::
is_a(Object klass) const
{
  Object result = protect(rb_obj_is_kind_of, *this, klass);
  return result.test();
}

inline bool Rice::Object::
respond_to(Identifier id) const
{
  return bool(rb_respond_to(*this, id));
}

inline bool Rice::Object::
is_instance_of(Object klass) const
{
  Object result = protect(rb_obj_is_instance_of, *this, klass);
  return result.test();
}

inline Rice::Object Rice::Object::
iv_get(
  Identifier name) const
{
  return protect(rb_ivar_get, *this, name.id());
}

inline Rice::Object Rice::Object::
attr_get(
  Identifier name) const
{
  return protect(rb_attr_get, *this, name.id());
}

inline void Rice::Object::
mark() const
{
  rb_gc_mark(*this);
}

inline void Rice::Object::
set_value(VALUE v)
{
  value_ = v;
}

inline bool Rice::
operator==(Rice::Object const& lhs, Rice::Object const& rhs)
{
  Object result = lhs.call("==", rhs);
  return result.test();
}

inline bool Rice::
operator!=(Rice::Object const& lhs, Rice::Object const& rhs)
{
  return !(lhs == rhs);
}

inline bool Rice::
operator<(Rice::Object const& lhs, Rice::Object const& rhs)
{
  Object result = lhs.call("<", rhs);
  return result.test();
}

inline bool Rice::
operator>(Rice::Object const& lhs, Rice::Object const& rhs)
{
  Object result = lhs.call(">", rhs);
  return result.test();
}

#endif // Rice__Object__ipp_

