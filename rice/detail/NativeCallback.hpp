#ifndef Rice__detail__Native_Callback__hpp_
#define Rice__detail__Native_Callback__hpp_

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallback
  {
  public:
    static Return_T call(Arg_Ts... args);
    //static NativeCallback<Return_T, Arg_Ts...> define(VALUE proc);
   // static void define(VALUE proc);

  public:
    // Disallow creating/copying/moving
    NativeCallback() = delete;
    NativeCallback(const NativeCallback&) = delete;
    NativeCallback(NativeCallback&&) = delete;
    void operator=(const NativeCallback&) = delete;
    void operator=(NativeCallback&&) = delete;

    static inline VALUE proc_ = Qnil;
  protected:
    NativeCallback(VALUE proc);
  };
}

#endif // Rice__detail__Native_Callback__hpp_
