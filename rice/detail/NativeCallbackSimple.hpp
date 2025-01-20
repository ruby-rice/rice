#ifndef Rice__detail__Native_Callback_Simple_hpp_
#define Rice__detail__Native_Callback_Simple_hpp_

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackSimple
  {
  public:
    static Return_T callback(Arg_Ts...args);
    static inline VALUE proc = Qnil;

  public:
    NativeCallbackSimple()  = delete;
    NativeCallbackSimple(const NativeCallbackSimple&) = delete;
    NativeCallbackSimple(NativeCallbackSimple&&) = delete;
    void operator=(const NativeCallbackSimple&) = delete;
    void operator=(NativeCallbackSimple&&) = delete;
  };
}
#endif // Rice__detail__Native_Callback_Simple_hpp_
