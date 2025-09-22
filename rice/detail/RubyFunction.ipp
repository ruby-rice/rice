
#include <any>
#include <tuple>

namespace Rice::detail
{
  template<typename Function_T, typename...Arg_Ts>
  inline RubyFunction<Function_T, Arg_Ts...>::RubyFunction(Function_T func, const Arg_Ts&... args)
    : func_(func), args_(std::forward_as_tuple(args...))
  {
  }

  template<typename Function_T, typename...Arg_Ts>
  inline typename RubyFunction<Function_T, Arg_Ts...>::Return_T RubyFunction<Function_T, Arg_Ts...>::operator()()
  {
    if constexpr (std::is_same_v<Return_T, void>)
    {
      std::apply(this->func_, this->args_);
    }
    else
    {
      this->result = std::apply(this->func_, this->args_);
      return result;
    }
  }
    
  // Create a functor for calling a Ruby function and define some aliases for readability.
  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args)
  {
    using Functor_T = RubyFunction<Function_T, Arg_Ts...>;
    using Return_T = typename Functor_T::Return_T;

    auto trampoline = [](VALUE value) -> VALUE
    {
      Functor_T* function = (Functor_T*)value;

      function->operator()();
      return Qnil;
    };

    // Create Invoker and call it via ruby
    int state = (int)JumpException::RUBY_TAG_NONE;
    Functor_T rubyFunction = RubyFunction<Function_T, Arg_Ts...>(func, std::forward<Arg_Ts>(args)...);
    rb_protect(trampoline, (VALUE)(&rubyFunction), &state);

    // Did anything go wrong?
    if (state == JumpException::RUBY_TAG_NONE)
    {
      if constexpr (!std::is_same_v<Return_T, void>)
      {
        return rubyFunction.result;
      }
    }
    else
    {
      VALUE err = rb_errinfo();
      if (state == JumpException::RUBY_TAG_RAISE && RB_TEST(err))
      {
        rb_set_errinfo(Qnil);
        throw Rice::Exception(err);
      }
      else
      {
        throw Rice::JumpException((Rice::JumpException::ruby_tag_type)state);
      }
    }
  }

  template<typename Function_T, typename ...Arg_Ts>
  auto no_gvl(Function_T func, Arg_Ts...args)
  {
    using Functor_T = RubyFunction<Function_T, Arg_Ts...>;
    using Return_T = typename Functor_T::Return_T;

    auto trampoline = [](void* functor) -> void*
    {
      Functor_T* function = (Functor_T*)functor;
      function->operator()();
      return nullptr;
    };

		// Create Invoker and call it via ruby
    Functor_T rubyFunction = RubyFunction<Function_T, Arg_Ts...>(func, std::forward<Arg_Ts>(args)...);
    rb_thread_call_without_gvl(trampoline, &rubyFunction, nullptr, nullptr);
    
    if constexpr (!std::is_same_v<Return_T, void>)
    {
      return rubyFunction.result;
    }
  }
}