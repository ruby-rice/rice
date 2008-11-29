#include "detail/define_method_and_auto_wrap.hpp"

template<typename Func_T>
void Rice::define_global_function(
    char const * name,
    Func_T func)
{
  Module(rb_mKernel).define_module_function(name, func);
}

