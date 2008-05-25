#ifndef Rice__detail__define_method_and_auto_wrap__ipp_
#define Rice__detail__define_method_and_auto_wrap__ipp_

#include "wrap_function.hpp"
#include "method_data.hpp"
#include "Exception_Handler.hpp"

template<typename Fun_T>
void Rice::detail::
define_method_and_auto_wrap(
    VALUE klass,
    Identifier name,
    Fun_T function,
    Exception_Handler const * handler)
{
  Data_Object<Wrapped_Function> f(
      wrap_function(function, handler),
      Data_Type<Wrapped_Function>(rb_cObject));
  protect(
      define_method_with_data,
      klass,
      name,
      f->func(),
      f->arity(),
      f);
}

#endif // Rice__detail__define_method_and_auto_wrap__ipp_
