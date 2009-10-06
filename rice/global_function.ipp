#include "detail/define_method_and_auto_wrap.hpp"

template<typename Func_T>
void Rice::define_global_function(
    char const * name,
    Func_T func,
    Arguments* arguments)
{
  Module(rb_mKernel).define_module_function(name, func, arguments);
}

template<typename Func_T>
void Rice::define_global_function(
    char const * name,
    Func_T func,
    Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  define_global_function(name, func, args);
}

