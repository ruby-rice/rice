#ifndef Rice__detail__Native_Callback_Simple_hpp_
#define Rice__detail__Native_Callback_Simple_hpp_

namespace Rice::detail
{
  template<typename Callback_T>
  class NativeCallbackSimple;

  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackSimple<Return_T(*)(Arg_Ts...)>
  {
  public:
    static Return_T callback(Arg_Ts...args);
    static inline VALUE proc = Qnil;
    static void setMethodInfo(MethodInfo* methodInfo);

  public:
    NativeCallbackSimple()  = delete;
    NativeCallbackSimple(const NativeCallbackSimple&) = delete;
    NativeCallbackSimple(NativeCallbackSimple&&) = delete;
    void operator=(const NativeCallbackSimple&) = delete;
    void operator=(NativeCallbackSimple&&) = delete;

  private:
    template<std::size_t... I>
    static Return_T callRuby(std::index_sequence<I...>& indices, Arg_Ts...args);
    static inline std::unique_ptr<MethodInfo> methodInfo_ = std::make_unique<MethodInfo>();
  };
}
#endif // Rice__detail__Native_Callback_Simple_hpp_
