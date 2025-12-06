#ifndef Rice__detail__Native_Callback_Ffi_hpp_
#define Rice__detail__Native_Callback_Ffi_hpp_

#ifdef HAVE_LIBFFI
#include <ffi.h>
#endif //HAVE_LIBFFI

namespace Rice::detail
{
  template<typename Callback_T>
  class NativeCallback;

  template<typename Return_T, typename ...Parameter_Ts>
  class NativeCallback<Return_T(*)(Parameter_Ts...)> : public Native
  {
  public:
    using Callback_T = Return_T(*)(Parameter_Ts...);
    using NativeCallback_T = NativeCallback<Callback_T>;
    using Tuple_T = std::tuple<Parameter_Ts...>;

    static VALUE finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);

    template<typename ...Arg_Ts>
    static void define(Arg_Ts&& ...args);

    static Return_T invoke(Parameter_Ts...args);
  public:
    NativeCallback(VALUE proc);
    ~NativeCallback();
    NativeCallback(const NativeCallback&) = delete;
    NativeCallback(NativeCallback&&) = delete;
    void operator=(const NativeCallback&) = delete;
    void operator=(NativeCallback&&) = delete;

    Callback_T callback();
  private:
    template<typename Parameter_T>
    static Parameter_T extractArg(void* arg);

    template<std::size_t... I>
    static Tuple_T convertArgsToTuple(void* args[], std::index_sequence<I...>& indices);
    Callback_T callback_ = nullptr;

    template<std::size_t... I>
    static void copyParametersImpl(std::vector<std::unique_ptr<ParameterAbstract>>& result, std::index_sequence<I...> indices);
    static std::vector<std::unique_ptr<ParameterAbstract>> copyParameters();
    static std::unique_ptr<Return> copyReturnInfo();

    static inline std::vector<std::unique_ptr<ParameterAbstract>> parameters_;
    static inline std::unique_ptr<Return> returnInfo_;
    static inline NativeCallback_T* native_;

  private:
    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

    template<std::size_t... I>
    Return_T callRuby(std::index_sequence<I...>& indices, Parameter_Ts...args);

    VALUE proc_;
    From_Ruby<Return_T> fromRuby_;

#ifdef HAVE_LIBFFI
    template <typename Arg_T>
    static ffi_type* ffiType();

    static void invokeFFI(ffi_cif* cif, void* ret, void* args[], void* instance);

    static inline std::array<ffi_type*, sizeof...(Parameter_Ts)> args_ = { ffiType<Parameter_Ts>()... };
    static inline ffi_cif cif_;
    ffi_closure* closure_ = nullptr;
#endif //HAVE_LIBFFI
  };
}
#endif // Rice__detail__Native_Callback_Ffi_hpp_
