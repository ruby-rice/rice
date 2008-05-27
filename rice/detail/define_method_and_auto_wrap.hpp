#ifndef Rice__detail__define_method_and_auto_wrap__hpp_
#define Rice__detail__define_method_and_auto_wrap__hpp_

#include "ruby.hpp"
#include "../Identifier.hpp"

namespace Rice
{

namespace detail
{

template<typename Fun_T>
void define_method_and_auto_wrap(
    VALUE klass,
    Identifier name,
    Fun_T function,
    Object handler);

} // detail

} // Rice

#include "define_method_and_auto_wrap.ipp"

#endif // Rice__detail__define_method_and_auto_wrap__hpp_
