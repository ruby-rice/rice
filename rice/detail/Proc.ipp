namespace Rice::detail
{
  // Note Return_T(Arg_Ts...) is intentional versus Return_T(*)(Arg_Ts...). That is
  // because the Type machinery strips all pointers/references/const/val etc to avoid
  // having an explosion of Type definitions
  template<typename Return_T, typename ...Arg_Ts>
  struct Type<Return_T(Arg_Ts...)>
  {
    static bool verify()
    {
      return true;
    }
  };

  // Wraps a C++ function as a Ruby proc
  template<typename Return_T, typename ...Arg_Ts>
  class To_Ruby<Return_T(*)(Arg_Ts...)>
  {
  public:
    using Proc_T = Return_T(*)(Arg_Ts...);

    To_Ruby() = default;

    explicit To_Ruby(Arg* arg)
    {
    }

    VALUE convert(Proc_T proc)
    {
      using NativeFunction_T = NativeFunction<void, Proc_T, false>;
      auto native = new NativeFunction_T(proc);
      VALUE result = rb_proc_new(NativeFunction_T::procEntry, (VALUE)native);

      // Tie the lifetime of the NativeCallback C++ instance to the lifetime of the Ruby proc object
      VALUE finalizer = rb_proc_new(NativeFunction_T::finalizerCallback, (VALUE)native);
      rb_define_finalizer(result, finalizer);

      return result;
    }
  };

  // Makes a Ruby proc callable as C callback
  template<typename Return_T, typename ...Arg_Ts>
  class From_Ruby<Return_T(*)(Arg_Ts...)>
  {
  public:
    using Callback_T = Return_T(*)(Arg_Ts...);

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
      using NativeCallback_T = NativeCallbackFFI<Return_T(*)(Arg_Ts...)>;
      NativeCallback_T* nativeCallback = new NativeCallback_T(value);

      // Tie the lifetime of the NativeCallback C++ instance to the lifetime of the Ruby proc object
      VALUE finalizer = rb_proc_new(NativeCallback_T::finalizerCallback, (VALUE)nativeCallback);
      rb_define_finalizer(value, finalizer);

      return nativeCallback->callback();
    }
#else
    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallbackSimple<Return_T(*)(Arg_Ts...)>;
      NativeCallback_T::proc = value;
      return &NativeCallback_T::callback;
    }
#endif

  private:
    Arg* arg_ = nullptr;
  };
}
