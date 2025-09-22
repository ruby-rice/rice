#ifndef Rice__detail__ruby_function__hpp_
#define Rice__detail__ruby_function__hpp_

namespace Rice::detail
{
  /* This is functor class that wraps calls to a Ruby C API method. It is needed because
     rb_protect only supports calling methods that take one argument. Thus 
     we invoke rb_protect telling it to invoke Ruby_Function::call with an 
     instance of a Ruby_Function. That instance then in turn calls the original
     Ruby method passing along its required arguments. */

  template<typename Function_T, typename...Arg_Ts>
  class RubyFunction
  {
  public:
    using Return_T = typename function_traits<Function_T>::return_type;
    std::conditional_t<std::is_void_v<Return_T>, int, Return_T> result;

  public:
    RubyFunction(Function_T func, const Arg_Ts&... args);
    Return_T operator()();

  private:
    Function_T func_;
    std::tuple<Arg_Ts...> args_;
  };

  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args);

  template<typename Function_T, typename ...Arg_Ts>
  auto no_gvl(Function_T func, Arg_Ts...args);
}

#endif // Rice__detail__ruby_function__hpp_