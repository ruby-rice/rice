#ifndef Rice__Object__ipp_
#define Rice__Object__ipp_

#include "protect.hpp"

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
void Rice::Object::
iv_set(
    Identifier name,
    T const & value)
{
  protect(rb_ivar_set, *this, name.id(), detail::To_Ruby<T>::convert(value));
}

#endif // Rice__Object__ipp_

