#ifndef Rice__detail__define_method_and_auto_wrap__ipp_
#define Rice__detail__define_method_and_auto_wrap__ipp_

#include "wrap_function.hpp"
#include "method_data.hpp"
#include "Exception_Handler_defn.hpp"
#include "../protect.hpp"

template<typename Fun_T>
void Rice::detail::
define_method_and_auto_wrap(
    VALUE klass,
    Identifier name,
    Fun_T function,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  Data_Object<Wrapped_Function> f(
      wrap_function(function, handler, arguments),
      rb_cObject);
  Rice::protect(
      define_method_with_data,
      klass,
      name.id(),
      f->func(),
      -1,
      f);
}

#endif // Rice__detail__define_method_and_auto_wrap__ipp_
