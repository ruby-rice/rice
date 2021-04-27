#include "cpp_api/Module.hpp"

template<typename Function_T>
void Rice::define_global_function(char const * name, Function_T&& func, MethodInfo* methodInfo)
{
  Module(rb_mKernel).define_module_function(name, std::forward<Function_T>(func), methodInfo);
}

template<typename Function_T, typename...Arg_Ts>
void Rice::define_global_function(char const * name, Function_T&& func, Arg_Ts const& ...args)
{
  Module(rb_mKernel).define_module_function(name, std::forward<Function_T>(func), args...);
}