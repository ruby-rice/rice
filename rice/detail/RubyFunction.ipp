#include "Jump_Tag.hpp"
#include "../Exception_defn.hpp"

#include <any>

namespace Rice::detail
{
  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline RubyFunction<Function_T, Return_T, Arg_Ts...>::RubyFunction(Function_T func, const Arg_Ts&... args)
    : func_(func), args_(std::forward_as_tuple(args...))
  {
  }

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  inline Return_T RubyFunction<Function_T, Return_T, Arg_Ts...>::operator()()
  {
    const int TAG_RAISE = 0x6; // From Ruby header files
    int state = 0;

    // Setup a thread local variable to capture the result of the Ruby function call.
    // We use thread_local because the lambda has to be captureless so it can
    // be converted to a function pointer callable by C.
    // The thread local variable avoids having to cast the result to VALUE and then 
    // back again to Return_T. The problem with that is the translation is not lossless
    // in some cases - for example a double with value of -1.0 does not roundrip.
    // 
    thread_local std::any result;

    // Callback that will invoke the Ruby function
    using Functor_T = RubyFunction<Function_T, Return_T, Arg_Ts...>;
    auto callback = [](VALUE value)
    {
      Functor_T* functor = (Functor_T*)value;

      if constexpr (std::is_same_v<Return_T, void>)
      {
        std::apply(functor->func_, functor->args_);
      }
      else
      {
        result = std::apply(functor->func_, functor->args_);
      }

      return Qnil;
    };

    // Now call rb_protect which will invoke the callback lambda above
    rb_protect(callback, (VALUE)this, &state);

    // Did anything go wrong?
    if (state == 0)
    {
      if constexpr (!std::is_same_v<Return_T, void>)
      {
        return std::any_cast<Return_T>(result);
      }
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
    
  // Create a functor for calling a Ruby function and define some aliases for readability.
  template<typename Return_T, typename ...Arg_Ts>
  inline Return_T protect(Return_T(*func)(Arg_Ts...), Arg_Ts...args)
  {
    using Function_T = Return_T(*)(Arg_Ts...);
    auto rubyFunction = RubyFunction<Function_T, Return_T, Arg_Ts...>(func, args...);
    return rubyFunction();
  }

  /*// Create a functor for calling a Ruby function that returns void
  template<typename ...Arg_Ts>
  inline void protect(void(*func)(Arg_Ts...), Arg_Ts...args)
  {
    using Function_T = void(*)(Arg_Ts...);
    auto rubyFunction = Ruby_Function<Function_T, void, Arg_Ts...>(func, args...);
    rubyFunction();
  }*/
}

namespace Rice
{
  template<typename Return_T, typename ...Arg_Ts>
  inline Return_T protect(Return_T(*func)(Arg_Ts...), Arg_Ts...args)
  {
    using Function_T = Return_T(*)(Arg_Ts...);
    auto rubyFunction = detail::RubyFunction<Function_T, Return_T, Arg_Ts...>(func, args...);
    return rubyFunction();
  }
}
