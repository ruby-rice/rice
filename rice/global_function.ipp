#include "detail/define_method_and_auto_wrap.hpp"

template<typename Func_T>
void Rice::define_global_function(
    char const * name,
    Func_T func)
{
  detail::define_method_and_auto_wrap(
      rb_cObject, name, func);
}

