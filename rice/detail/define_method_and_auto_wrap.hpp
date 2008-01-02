#ifndef Rice__detail__define_method_and_auto_wrap__hpp_
#define Rice__detail__define_method_and_auto_wrap__hpp_

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

class Exception_Handler;

template<typename Fun_T>
void define_method_and_auto_wrap(
    VALUE klass,
    char const * name,
    Fun_T function,
    Exception_Handler const * handler = 0);

} // detail

} // Rice

#include "define_method_and_auto_wrap.ipp"

#endif // Rice__detail__define_method_and_auto_wrap__hpp_
