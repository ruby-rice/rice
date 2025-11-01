#ifndef Rice__detail__Native_Proc__hpp_
#define Rice__detail__Native_Proc__hpp_

namespace Rice::detail
{
  template<typename Proc_T>
  class NativeProc: Native
  {
  public:
    using NativeProc_T = NativeProc<Proc_T>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename function_traits<Proc_T>::return_type;
    using Parameter_Ts = typename function_traits<Proc_T>::arg_types;
    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Define a new Ruby Proc to wrap a C++ function
    static VALUE createRubyProc(Proc_T proc);
    static NativeProc<Proc_T>* define(Proc_T proc);

    // This is the method Ruby calls when invoking the proc
    static VALUE resolve(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
 
  public:
    NativeProc(Proc_T proc, MethodInfo* methodInfo);
    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;
    
    std::string name() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

  private:
    static VALUE finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Parameter_Ts getNativeValues(std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices);

    // Call the underlying C++ function
    VALUE invoke(Parameter_Ts&& nativeArgs);

  private:
    Proc_T proc_;
    std::unique_ptr<MethodInfo> methodInfo_;
    To_Ruby<To_Ruby_T> toRuby_;
  };
}

#endif // Rice__detail__Native_Proc__hpp_
