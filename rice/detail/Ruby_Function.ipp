#include "../Exception_defn.hpp"

namespace Rice::detail
{
  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline Ruby_Function<Function_T, Return_T, Arg_Ts...>::Ruby_Function(Function_T func, const Arg_Ts&... args)
    : func_(func), args_(std::forward_as_tuple(args...))
  {
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline VALUE Ruby_Function<Function_T, Return_T, Arg_Ts...>::operator()()
  {
    const int TAG_RAISE = 0x6; // From Ruby header files
    int state = 0;

    // Call our invoke function via rb_protect to capture any ruby exceptions
    RUBY_VALUE_FUNC callback = (RUBY_VALUE_FUNC)&Ruby_Function<Function_T, Return_T, Arg_Ts...>::invoke;
    VALUE retval = rb_protect(callback, (VALUE)this, &state);

    // Did anythoing go wrong?
    if (state == 0)
    {
      return retval;
    }
    else
    {
      VALUE err = rb_errinfo();
      if (state == TAG_RAISE && RTEST(err))
      {
        rb_set_errinfo(Qnil);
        throw Rice::Exception(err);
      }
      else
      {
        throw Jump_Tag(state);
      }
    }
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline VALUE Ruby_Function<Function_T, Return_T, Arg_Ts...>::invoke(VALUE value)
  {
    Ruby_Function<Function_T, Return_T, Arg_Ts...>* instance = (Ruby_Function<Function_T, Return_T, Arg_Ts...>*)value;
    
    if constexpr (std::is_same_v<Return_T, void>)
    {
      std::apply(instance->func_, instance->args_);
      return Qnil;
    }
    else
    {
      return std::apply(instance->func_, instance->args_);
    }
  }

  template<typename Function_T, typename... Arg_Ts>
  inline decltype(auto) Make_Ruby_Function(Function_T&& func, Arg_Ts&&... args)
  {
    using Return_T = std::invoke_result_t<decltype(func), Arg_Ts...>;
    return Ruby_Function<Function_T, Return_T, Arg_Ts...>(std::forward<Function_T>(func), std::forward<Arg_Ts>(args)...);
  }

  template<typename Function_T, typename ...Arg_Ts>
  inline VALUE protect(Function_T&& func, Arg_Ts&&... args)
  {
    // Create a functor for calling a Ruby function and define some aliases for readability.
    auto rubyFunction = detail::Make_Ruby_Function(std::forward<Function_T>(func), std::forward<Arg_Ts>(args)...);
    return rubyFunction();
  }
}

namespace Rice
{
  template<typename Function_T, typename ...Arg_Ts>
  inline VALUE protect(Function_T&& func, Arg_Ts&&... args)
  {
    // Create a functor for calling a Ruby function and define some aliases for readability.
    auto rubyFunction = detail::Make_Ruby_Function(std::forward<Function_T>(func), std::forward<Arg_Ts>(args)...);
    return rubyFunction();
  }
}