namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackSimple<Return_T, Arg_Ts...>::callback(Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    VALUE result = detail::protect(rb_funcallv, proc, id.id(), (int)sizeof...(Arg_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby;
      return fromRuby.convert(result);
    }
  }
}