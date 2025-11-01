namespace Rice::detail
{
  template<typename Return_T, typename ...Parameter_Ts>
  struct Type<Return_T(*)(Parameter_Ts...)>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cProc;
    }
  };

  // Wraps a C++ function as a Ruby proc
  template<typename Return_T, typename ...Parameter_Ts>
  class To_Ruby<Return_T(*)(Parameter_Ts...)>
  {
  public:
    using Proc_T = Return_T(*)(Parameter_Ts...);

    To_Ruby() = default;

    explicit To_Ruby(Arg* arg)
    {
    }

    VALUE convert(Proc_T proc)
    {
      // Wrap the C+++ function pointer as a Ruby Proc
      return NativeProc<Proc_T>::createRubyProc(std::forward<Proc_T>(proc));
    }
  };

  // Makes a Ruby proc callable as C callback
  template<typename Return_T, typename ...Parameter_Ts>
  class From_Ruby<Return_T(*)(Parameter_Ts...)>
  {
  public:
    using Callback_T = Return_T(*)(Parameter_Ts...);

    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      if (protect(rb_obj_is_proc, value) == Qtrue || protect(rb_proc_lambda_p, value))
      {
        return Convertible::Exact;
      }
      else
      {
        return Convertible::None;
      }
    }

#ifdef HAVE_LIBFFI
    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallbackFFI<Return_T(*)(Parameter_Ts...)>;
      NativeCallback_T* nativeCallback = new NativeCallback_T(value);

      // Tie the lifetime of the NativeCallback C++ instance to the lifetime of the Ruby proc object
      VALUE finalizer = rb_proc_new(NativeCallback_T::finalizerCallback, (VALUE)nativeCallback);
      rb_define_finalizer(value, finalizer);

      return nativeCallback->callback();
    }
#else
    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallbackSimple<Return_T(*)(Parameter_Ts...)>;
      NativeCallback_T::proc = value;
      return &NativeCallback_T::callback;
    }
#endif

  private:
    Arg* arg_ = nullptr;
  };
}
