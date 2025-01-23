namespace Rice
{
  template<typename Callback_T, typename...Arg_Ts>
  void define_callback(const Arg_Ts&...args)
  {
    MethodInfo* methodInfo = new MethodInfo(detail::function_traits<Callback_T>::arity, args...);
  #ifdef HAVE_LIBFFI
    detail::NativeCallbackFFI<Callback_T>::setMethodInfo(methodInfo);
  #else
    detail::NativeCallbackSimple<Callback_T>::setMethodInfo(methodInfo);
  #endif
  }
}