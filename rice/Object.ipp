#ifndef Rice__Object__ipp_
#define Rice__Object__ipp_

#include "protect.hpp"
#include "detail/ruby.hpp"
#include "to_from_ruby.hpp"

#include <vector>

template<typename ...ArgT>
inline Rice::Object Rice::Object::
call(Identifier id, ArgT... args) const
{
  auto asList = this->convert_args<ArgT...>(args...);
  return protect(rb_funcall2, value(), id, asList.size(), &asList[0]);
}

template<typename ...ArgT>
std::vector<VALUE> Rice::Object::convert_args(ArgT&... args) const {
  return std::vector<VALUE>{ to_ruby(args)... };
}

template<typename T>
void Rice::Object::
iv_set(
    Identifier name,
    T const & value)
{
  protect(rb_ivar_set, *this, name.id(), to_ruby(value));
}

#endif // Rice__Object__ipp_

