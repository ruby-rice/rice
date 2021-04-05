#include "Module.hpp"

template<typename Func_T>
void Rice::define_global_function(char const * name, Func_T&& func, MethodInfo* methodInfo)
{
  Module(rb_mKernel).define_module_function(name, std::forward<Func_T>(func), methodInfo);
}

template<typename Func_T, typename...Arg_Ts>
void Rice::define_global_function(char const * name, Func_T&& func, Arg_Ts const& ...args)
{
  Module(rb_mKernel).define_module_function(name, std::forward<Func_T>(func), args...);
}