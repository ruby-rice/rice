RICE_BEGIN_NAMESPACE
  template<typename Callback_T, typename...Arg_Ts>
  inline void define_callback(Arg_Ts&&...args)
  {
    detail::NativeCallback<Callback_T>::define(std::forward<Arg_Ts>(args)...);
  }
RICE_END_NAMESPACE
