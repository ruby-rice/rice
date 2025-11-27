#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <tuple>

namespace Rice::detail
{
  template<typename Proc_T>
  NativeProc<Proc_T>* NativeProc<Proc_T>::define(Proc_T proc)
  {
    // Create proc parameters
    std::vector<std::unique_ptr<ParameterAbstract>> parameters = Native::create_parameters<Parameter_Ts>();

    // Create return info
    std::unique_ptr<Return> returnInfo = std::make_unique<Return>();

    return new NativeProc_T(std::forward<Proc_T>(proc), std::move(returnInfo), std::move(parameters));
  }

  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::createRubyProc(Proc_T proc)
  {
    NativeProc_T* nativeProc = NativeProc_T::define(std::forward<Proc_T>(proc));

    // Create a Ruby proc to wrap it and pass the NativeProc as a callback parameter
    VALUE result = rb_proc_new(NativeProc_T::resolve, (VALUE)nativeProc);

    // Tie the lifetime of the NativeProc to the Ruby Proc
    VALUE finalizer = rb_proc_new(NativeProc_T::finalizerCallback, (VALUE)nativeProc);
    rb_define_finalizer(result, finalizer);

    return result;
  }

  // Ruby calls this method when invoking a proc that was defined as a C++ function
  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::resolve(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    return cpp_protect([&]
    {
      NativeProc_T * native = (NativeProc_T*)callback_arg;
      return (*native)(argc, argv, Qnil);
    });
  }

  // Ruby calls this method if an instance of a NativeProc is owned by a Ruby proc. That happens when C++
  // returns a function back to Ruby
  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    NativeProc_T* native = (NativeProc_T*)callback_arg;
    delete native;
    return Qnil;
  }

  template<typename Proc_T>
  NativeProc<Proc_T>::NativeProc(Proc_T proc, std::unique_ptr<Return>&& returnInfo, std::vector<std::unique_ptr<ParameterAbstract>>&& parameters)
    : Native("proc", std::move(returnInfo), std::move(parameters)),
      proc_(proc), toRuby_(returnInfo_.get())
  {
  }

  template<typename Proc_T>
  std::string NativeProc<Proc_T>::toString()
  {
    return "Proc";
  }

  template<typename Proc_T>
  template<std::size_t... I>
  typename NativeProc<Proc_T>::Parameter_Ts NativeProc<Proc_T>::getNativeValues(std::vector<std::optional<VALUE>>& values,
     std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Parameter_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values 
       which we let the compiler convert to const values as needed. This works except for 
       T** -> const T**, see comment in convertToNative method. */
    //return std::forward_as_tuple(this->getNativeValue<std::tuple_element_t<I, Parameter_Ts>, I>(values)...);
    return std::forward_as_tuple(
      (dynamic_cast<Parameter<std::tuple_element_t<I, Parameter_Ts>>*>(this->parameters_[I].get()))->
               convertToNative(values[I])...);
  }

  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::invoke(Parameter_Ts&& nativeArgs)
  {
    if constexpr (std::is_void_v<Return_T>)
    {
      std::apply(this->proc_, std::forward<Parameter_Ts>(nativeArgs));
      return Qnil;
    }
    else
    {
      // Call the native method and get the result
      Return_T nativeResult = std::apply(this->proc_, std::forward<Parameter_Ts>(nativeArgs));

      // Return the result
      return this->toRuby_.convert(std::forward<Return_T>(nativeResult));
    }
  }

  template<typename Proc_T>
  VALUE NativeProc<Proc_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    // Get the ruby values and make sure we have the correct number
    std::vector<std::optional<VALUE>> rubyValues = this->getRubyValues(argc, argv, true);

    auto indices = std::make_index_sequence<std::tuple_size_v<Parameter_Ts>>{};

    // Convert the Ruby values to native values
    Parameter_Ts nativeValues = this->getNativeValues(rubyValues, indices);

    // Now call the native method
    VALUE result = this->invoke(std::forward<Parameter_Ts>(nativeValues));

    return result;
  }

  template<typename Proc_T>
  inline NativeKind NativeProc< Proc_T>::kind()
  {
    return NativeKind::Proc;
  }

  template<typename Proc_T>
  inline VALUE NativeProc<Proc_T>::returnKlass()
  {
    TypeMapper<Return_T> typeMapper;
    return typeMapper.rubyKlass();
  }
}