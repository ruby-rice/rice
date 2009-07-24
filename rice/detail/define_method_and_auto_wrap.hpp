#ifndef Rice__detail__define_method_and_auto_wrap__hpp_
#define Rice__detail__define_method_and_auto_wrap__hpp_

#include "ruby.hpp"
#include "../Data_Object.hpp"
#include "../Identifier.hpp"
#include "Arguments.hpp"

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
    Data_Object<Exception_Handler> handler,
    Arguments* arguments = 0);

} // detail

} // Rice

#include "define_method_and_auto_wrap.ipp"

#endif // Rice__detail__define_method_and_auto_wrap__hpp_
