namespace Rice::detail
{
  // ----- ResultWrapper -------
  template<typename Return_T>
  inline Return_T ResultWrapper<Return_T>::getResult()
  {
    if constexpr (!std::is_copy_constructible_v<Return_T> && std::is_move_constructible_v<Return_T>)
    {
      return std::move(this->result_.value());
    }
    // std::is_copy_constructible_v<std::vector<std::unique_ptr<T>>>> return true. Sigh.
    else if constexpr (detail::is_std_vector_v<Return_T> && std::is_copy_constructible_v<Return_T>)
    {
      if constexpr (!std::is_copy_constructible_v<typename Return_T::value_type>)
      {
        return std::move(this->result_.value());
      }
      else
      {
        return this->result_.value();
      }
    }
    else
    {
      return this->result_.value();
    }
	}

  template<typename Return_T>
  inline void ResultWrapper<Return_T>::setResult(Return_T&& value)
  {
    if constexpr(std::is_copy_assignable_v<Return_T> || std::is_move_assignable_v<Return_T>)
    {
      this->result_ = std::forward<Return_T>(value);
    }
    else if constexpr (std::is_copy_constructible_v<Return_T>)
    {
      this->result_.emplace(value);
    }
    else if constexpr (std::is_move_constructible_v<Return_T>)
    {
      this->result_.emplace(std::move(value));
    }
  }

  template<typename Return_T>
  inline Return_T& ResultWrapper<Return_T&>::getResult()
  {
    return this->result_.value().get();
  }

  template<typename Return_T>
  inline void ResultWrapper<Return_T&>::setResult(Return_T& value)
  {
    this->result_ = value;
  }

  template<typename Return_T>
  inline Return_T* ResultWrapper<Return_T*>::getResult()
  {
    return this->result_;
  }

  template<typename Return_T>
  inline void ResultWrapper<Return_T*>::setResult(Return_T* value)
  {
    this->result_ = value;
  }

  // ----- Invoker -------
  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline NativeInvoker<Return_T, Function_T, Tuple_T>::NativeInvoker(const Function_T func, Tuple_T&& args)
    : func(func), args(std::forward<Tuple_T>(args))
  {
  }

  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline void NativeInvoker<Return_T, Function_T, Tuple_T>::invoke()
  {
    try
    {
      if constexpr (std::is_void_v<Return_T>)
      {
        std::apply(this->func, std::forward<Tuple_T>(this->args));
      }
      else if constexpr (!std::is_pointer_v<Return_T> && !std::is_reference_v<Return_T> && !std::is_copy_assignable_v<Return_T> && std::is_move_assignable_v<Return_T>)
      {
        this->resultWrapper.setResult(std::move(std::apply(this->func, std::forward<Tuple_T>(this->args))));
      }
      else
      {
        this->resultWrapper.setResult(std::forward<Return_T>(std::apply(this->func, std::forward<Tuple_T>(this->args))));
      }
    }
    catch (...)
    {
      this->resultWrapper.exception = std::current_exception();
    }
  }

  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline Return_T NativeInvoker<Return_T, Function_T, Tuple_T>::result()
  {
		return this->resultWrapper.getResult();
  }

  template<typename Return_T, typename Function_T, typename Tuple_T>
  inline std::exception_ptr NativeInvoker<Return_T, Function_T, Tuple_T>::exception()
  {
    return this->resultWrapper.exception;
  }

  // ------- Helper Methods --------
  // Create a functor for calling a Ruby function and define some aliases for readability.
  template<typename Function_T, typename ...Arg_Ts>
  auto protect(Function_T func, Arg_Ts...args)
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Tuple_T = std::tuple<Arg_Ts...>;
    using Invoker_T = NativeInvoker<Return_T, Function_T, Tuple_T>;

    Tuple_T argsTuple = std::forward_as_tuple(args...);
    Invoker_T invoker(func, std::forward<Tuple_T>(argsTuple));

    auto trampoline = [](VALUE value) -> VALUE
      {
        Invoker_T* function = (Invoker_T*)value;

        function->invoke();
        return Qnil;
      };

    // Create Invoker and call it via ruby
    int state = (int)JumpException::RUBY_TAG_NONE;
    rb_protect(trampoline, (VALUE)(&invoker), &state);

    // Did anything go wrong?
    if (state == JumpException::RUBY_TAG_NONE)
    {
      if constexpr (!std::is_same_v<Return_T, void>)
      {
        return invoker.result();
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
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Arg_Ts...args)
  {
    using Tuple_T = std::tuple<Arg_Ts...>;
    Tuple_T argsTuple = std::forward_as_tuple(args...);
    return no_gvl(func, argsTuple);
  }

  template<typename Function_T, typename Tuple_T>
  typename function_traits<Function_T>::return_type no_gvl(Function_T func, Tuple_T&& args)
  {
    using Return_T = typename function_traits<Function_T>::return_type;
    using Invoker_T = NativeInvoker<Return_T, Function_T, Tuple_T>;

    Invoker_T invoker(func, std::forward<Tuple_T>(args));

    auto trampoline = [](void* functor) -> void*
      {
        Invoker_T* function = (Invoker_T*)functor;
        function->invoke();

        if (function->exception())
        {
          std::rethrow_exception(function->exception());
        }

        return nullptr;
      };

    // Create Invoker and call it via ruby
    rb_thread_call_without_gvl(trampoline, &invoker, nullptr, nullptr);

    if constexpr (!std::is_same_v<Return_T, void>)
    {
      return invoker.result();
    }
  }
}