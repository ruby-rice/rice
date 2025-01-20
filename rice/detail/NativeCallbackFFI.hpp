#ifndef Rice__detail__Native_Callback_Ffi_hpp_
#define Rice__detail__Native_Callback_Ffi_hpp_

#ifdef HAVE_LIBFFI

#include <ffi.h>

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  class NativeCallbackFFI
  {
  public:
    using Callback_T = Return_T(Arg_Ts...);
    using Tuple_T = std::tuple<Arg_Ts...>;
    static void ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance);
  public:
    NativeCallbackFFI(VALUE proc);
    NativeCallbackFFI(const NativeCallbackFFI&) = delete;
    NativeCallbackFFI(NativeCallbackFFI&&) = delete;
    void operator=(const NativeCallbackFFI&) = delete;
    void operator=(NativeCallbackFFI&&) = delete;

    Return_T operator()(Arg_Ts...args);
    Callback_T* callback();

  private:
    template <typename Arg_T>
    static ffi_type* ffiType();

    template<std::size_t... I>
    static Tuple_T convertArgsToTuple(void* args[], std::index_sequence<I...>& indices);

    static inline std::array<ffi_type*, sizeof...(Arg_Ts)> args_ = { ffiType<Arg_Ts>()... };
    static inline ffi_cif cif_;
    static inline ffi_closure* closure_ = nullptr;
    static inline Callback_T* callback_ = nullptr;
 
  private:
    VALUE proc_;
  };
}
#endif // HAVE_LIBFFI

#endif // Rice__detail__Native_Callback_Ffi_hpp_
