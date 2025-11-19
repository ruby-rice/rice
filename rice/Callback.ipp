namespace Rice
{
  template<typename Callback_T, typename...Arg_Ts>
  void define_callback(Arg_Ts&...args)
  {
    detail::NativeCallback<Callback_T>::define(std::forward<Arg_Ts...>(args...));
  }
}