#ifndef Rice__detail__ruby_function__hpp_
#define Rice__detail__ruby_function__hpp_

#include "ruby.hpp"
#include "../to_from_ruby_defn.hpp"

namespace Rice
{

namespace detail
{
  /* This is functor class that wraps calls to a Ruby C API method. It is needed because
     rb_protect only supports calling methods that take one argument. Thus 
     we invoke rb_protect telling it to invoke Ruby_Function::call with an 
     instance of a Ruby_Function. That instance then in turn calls the original
     Ruby method passing along its required arguments. */

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  class Ruby_Function
  {
  public:
    static VALUE call(Ruby_Function* rubyFunction);

  public:
    Ruby_Function(Function_T func, const Arg_Ts&... args);
    Return_T operator()();

  private:
    Function_T func_;
    std::tuple<Arg_Ts...> args_;
  };

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline VALUE Ruby_Function<Function_T, Return_T, Arg_Ts...>::
    call(Ruby_Function* rubyFunction)
  {
    Object result(rubyFunction->operator()());
    return result.value();
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  Ruby_Function<Function_T, Return_T, Arg_Ts...>::
    Ruby_Function(Function_T func, const Arg_Ts&... args)
    : func_(func), args_(std::forward_as_tuple(args...))
  {
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  Return_T Ruby_Function<Function_T, Return_T, Arg_Ts...>::
    operator()()
  {
    return std::apply(this->func_, this->args_);
  }
} // namespace detail


namespace detail
{
    template<typename Function_T, typename... Arg_Ts>
    decltype(auto) ruby_function(Function_T func, Arg_Ts&&... args)
    {
      using Return_T = std::invoke_result_t<decltype(func), Arg_Ts...>;
      return Ruby_Function<Function_T, Return_T, Arg_Ts...>(func, std::forward<Arg_Ts>(args)...);
    }


} // namespace detail

} // namespace rice


#endif // Rice__detail__ruby_function__hpp_

