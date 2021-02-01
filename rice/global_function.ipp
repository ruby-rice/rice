#include "Module.hpp"

template<typename Func_T>
void Rice::define_global_function(
    char const * name,
    Func_T func,
    Arguments* arguments)
{
  Module(rb_mKernel).define_module_function(name, func, arguments);
}

template<typename Func_T, typename...Arg_Ts>
void Rice::define_global_function(
    char const * name,
    Func_T func,
    Arg_Ts const& ...args)
{
  Module(rb_mKernel).define_module_function(name, func, args...);
}

