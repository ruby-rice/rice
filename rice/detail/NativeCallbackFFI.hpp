#ifndef Rice__detail__Native_Callback_Ffi_hpp_
#define Rice__detail__Native_Callback_Ffi_hpp_

#ifdef HAVE_LIBFFI

#include <ffi.h>

namespace Rice::detail
{
  template<typename Callback_T>
  class NativeCallbackFFI;

  template<typename Return_T, typename ...Parameter_Ts>
  class NativeCallbackFFI<Return_T(*)(Parameter_Ts...)>
  {
  public:
    using Callback_T = Return_T(Parameter_Ts...);
    using Tuple_T = std::tuple<Parameter_Ts...>;
    static void ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance);
    static VALUE finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
    static void setMethodInfo(MethodInfo* methodInfo);

  public:
    NativeCallbackFFI(VALUE proc);
    ~NativeCallbackFFI();
    NativeCallbackFFI(const NativeCallbackFFI&) = delete;
    NativeCallbackFFI(NativeCallbackFFI&&) = delete;
    void operator=(const NativeCallbackFFI&) = delete;
    void operator=(NativeCallbackFFI&&) = delete;

    Return_T operator()(Parameter_Ts...args);
    Callback_T* callback();

  private:
    template <typename Arg_T>
    static ffi_type* ffiType();

    template<std::size_t... I>
    static Tuple_T convertArgsToTuple(void* args[], std::index_sequence<I...>& indices);

    static inline std::array<ffi_type*, sizeof...(Parameter_Ts)> args_ = { ffiType<Parameter_Ts>()... };
    static inline ffi_cif cif_;
    static inline ffi_closure* closure_ = nullptr;
    static inline Callback_T* callback_ = nullptr;
    static inline std::unique_ptr<MethodInfo> methodInfo_ = std::make_unique<MethodInfo>();

    template<std::size_t... I>
    Return_T callRuby(std::index_sequence<I...>& indices, Parameter_Ts...args);
  private:
    VALUE proc_;
  };
}
#endif // HAVE_LIBFFI

#endif // Rice__detail__Native_Callback_Ffi_hpp_
