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
      if (protect(rb_obj_is_proc, value) == Qtrue || protect(rb_proc_lambda_p, value) == Qtrue)
      {
        return Convertible::Exact;
      }
      else
      {
        return Convertible::None;
      }
    }

    Callback_T convert(VALUE value)
    {
      using NativeCallback_T = NativeCallback<Callback_T>;
      NativeCallback_T* callback = new NativeCallback_T(value);
      return callback->callback();
    }

  private:
    Arg* arg_ = nullptr;
  };
}
