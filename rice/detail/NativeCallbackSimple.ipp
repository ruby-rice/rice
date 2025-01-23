namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  void NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::setMethodInfo(MethodInfo* methodInfo)
  {
    methodInfo_.reset(methodInfo);
  }

  template<typename Return_T, typename ...Arg_Ts>
  template<std::size_t... I>
  Return_T NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::callRuby(std::index_sequence<I...>& indices, Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>(methodInfo_->arg(I)).convert(args)... };
    VALUE result = detail::protect(rb_funcallv, proc, id.id(), (int)sizeof...(Arg_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby(dynamic_cast<Arg*>(&methodInfo_->returnInfo));
      return fromRuby.convert(result);
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::callback(Arg_Ts...args)
  {
    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};
    return NativeCallbackSimple<Return_T(*)(Arg_Ts...)>::callRuby(indices, args...);
  }
}