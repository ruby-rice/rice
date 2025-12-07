
template<typename Function_T, typename...Arg_Ts>
void RICE_PREPEND_NAMESPACE(define_global_function)(char const * name, Function_T&& func, Arg_Ts const& ...args)
{
  Module(rb_mKernel).define_module_function(name, std::forward<Function_T>(func), args...);
}
