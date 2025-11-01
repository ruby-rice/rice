namespace Rice::detail
{
  template<typename Return_T, typename ...Parameter_Ts>
  void NativeCallbackSimple<Return_T(*)(Parameter_Ts...)>::setMethodInfo(MethodInfo* methodInfo)
  {
    methodInfo_.reset(methodInfo);
  }

  template<typename Return_T, typename ...Parameter_Ts>
  template<std::size_t... I>
  Return_T NativeCallbackSimple<Return_T(*)(Parameter_Ts...)>::callRuby(std::index_sequence<I...>& indices, Parameter_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Parameter_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Parameter_Ts>>(methodInfo_->arg(I)).convert(args)... };
    VALUE result = detail::protect(rb_funcallv, proc, id.id(), (int)sizeof...(Parameter_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby(dynamic_cast<Arg*>(methodInfo_->returnInfo()));
      return fromRuby.convert(result);
    }
  }

  template<typename Return_T, typename ...Parameter_Ts>
  Return_T NativeCallbackSimple<Return_T(*)(Parameter_Ts...)>::callback(Parameter_Ts...args)
  {
    auto indices = std::make_index_sequence<sizeof...(Parameter_Ts)>{};
    return NativeCallbackSimple<Return_T(*)(Parameter_Ts...)>::callRuby(indices, args...);
  }
}