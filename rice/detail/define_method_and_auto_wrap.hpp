#ifndef Rice__detail__define_method_and_auto_wrap__hpp_
#define Rice__detail__define_method_and_auto_wrap__hpp_

#include "ruby.hpp"
#include "../Data_Object.hpp"
#include "../Identifier.hpp"

namespace Rice
{

namespace detail
{

class Exception_Handler;

template<typename Fun_T>
void define_method_and_auto_wrap(
    VALUE klass,
    Identifier name,
    Fun_T function,
    Data_Object<Exception_Handler> handler);

} // detail

} // Rice

#include "define_method_and_auto_wrap.ipp"

#endif // Rice__detail__define_method_and_auto_wrap__hpp_
