#ifdef HAVE_LIBFFI
#include <ffi.h>

namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  template<typename Arg_T>
  ffi_type* NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::ffiType()
  {
    std::map<std::type_index, ffi_type*> nativeToFfiMapping = { 
      {std::type_index(typeid(bool)),               &ffi_type_uint8},
      {std::type_index(typeid(bool)),               &ffi_type_uint8},
      {std::type_index(typeid(char)),               &ffi_type_schar},
      {std::type_index(typeid(unsigned char)),      &ffi_type_uchar},
      {std::type_index(typeid(signed char)),        &ffi_type_schar},
      {std::type_index(typeid(uint8_t)),            &ffi_type_uint8},
      {std::type_index(typeid(unsigned short)),     &ffi_type_uint8},
      {std::type_index(typeid(int8_t)),             &ffi_type_sint8},
      {std::type_index(typeid(short)),              &ffi_type_sint8},
      {std::type_index(typeid(uint16_t)),           &ffi_type_uint16},
      {std::type_index(typeid(int16_t)),            &ffi_type_sint16},
      {std::type_index(typeid(uint32_t)),           &ffi_type_uint32},
      {std::type_index(typeid(unsigned int)),       &ffi_type_uint32},
      {std::type_index(typeid(signed int)),         &ffi_type_uint32},
      {std::type_index(typeid(int32_t)),            &ffi_type_sint32},
      {std::type_index(typeid(uint64_t)),           &ffi_type_uint64},
      {std::type_index(typeid(unsigned long long)), &ffi_type_uint64},
      {std::type_index(typeid(int64_t)),            &ffi_type_sint64},
      {std::type_index(typeid(signed long long)),   &ffi_type_sint64},
      {std::type_index(typeid(float)),              &ffi_type_float},
      {std::type_index(typeid(double)),             &ffi_type_double},
      {std::type_index(typeid(void)),               &ffi_type_pointer},
      {std::type_index(typeid(long double)),        &ffi_type_longdouble}
    };

    if (sizeof(long) == 32)
    {
      nativeToFfiMapping[std::type_index(typeid(unsigned long))] = &ffi_type_uint32;
      nativeToFfiMapping[std::type_index(typeid(long))] = &ffi_type_sint32;
    }
    else if (sizeof(long) == 64)
    {
      nativeToFfiMapping[std::type_index(typeid(unsigned long))] = &ffi_type_uint64;
      nativeToFfiMapping[std::type_index(typeid(long))] = &ffi_type_sint64;
    }
    
    if (std::is_pointer_v<Arg_T>)
    {
      return &ffi_type_pointer;
    }
    else
    {
      const std::type_index& key = std::type_index(typeid(Arg_T));
      return nativeToFfiMapping[key];
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  template<std::size_t... I>
  typename NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::Tuple_T NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::convertArgsToTuple(void* args[], std::index_sequence<I...>& indices)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Arg_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values
       which we let the compiler convert to const values as needed. This works except for
       T** -> const T**, see comment in getNativeValue method. */
    return std::forward_as_tuple(*(std::tuple_element_t<I, Tuple_T>*)(args[I])...);
  }

  template<typename Return_T, typename ...Arg_Ts>
  void NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::ffiCallback(ffi_cif* cif, void* ret, void* args[], void* instance)
  {
    using Self_T = NativeCallbackFFI<Return_T(*)(Arg_Ts...)>;
    Self_T* self = (Self_T*)instance;

    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};

    if constexpr (sizeof...(Arg_Ts) == 0)
    {
      *(Return_T*)ret = self->operator()();
    }
    else
    {
      std::tuple<Arg_Ts...> tuple = convertArgsToTuple(args, indices);
      *(Return_T*)ret = std::apply(*self, tuple);
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  void NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::setMethodInfo(MethodInfo* methodInfo)
  {
    methodInfo_.reset(methodInfo);
  }

  template<typename Return_T, typename ...Arg_Ts>
  NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::~NativeCallbackFFI()
  {
    this->proc_ = Qnil;
  }

  template<typename Return_T, typename ...Arg_Ts>
  VALUE NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg)
  {
    using NativeCallback_T = NativeCallbackFFI<Return_T(*)(Arg_Ts...)>;
    NativeCallback_T* nativeCallback = (NativeCallback_T*)callback_arg;
    delete nativeCallback;
    return Qnil;
  }

  template<typename Return_T, typename ...Arg_Ts>
  NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::NativeCallbackFFI(VALUE proc) : proc_(proc)
  {
    // First setup desccription of callback
    if (cif_.bytes == 0)
    {
      ffi_prep_cif(&cif_, FFI_DEFAULT_ABI, sizeof...(Arg_Ts), &ffi_type_pointer, args_.data());
    }

    // Create FFI closure
    this->closure_ = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure) + sizeof(void*), (void**)(&this->callback_));
    ffi_status status = ffi_prep_closure_loc(this->closure_, &cif_, ffiCallback, (void*)this, (void*)this->callback_);
  }

  template<typename Return_T, typename ...Arg_Ts>
  typename NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::Callback_T* NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::callback()
  {
    return (Callback_T*)this->callback_;
  }

  template<typename Return_T, typename ...Arg_Ts>
  template<std::size_t... I>
  Return_T NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::callRuby(std::index_sequence<I...>& indices, Arg_Ts...args)
  {
    static Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>(methodInfo_->arg(I)).convert(args)... };
    VALUE result = detail::protect(rb_funcallv, this->proc_, id.id(), (int)sizeof...(Arg_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby(dynamic_cast<Arg*>(&methodInfo_->returnInfo));
      return fromRuby.convert(result);
    }
  }

  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::operator()(Arg_Ts...args)
  {
    auto indices = std::make_index_sequence<sizeof...(Arg_Ts)>{};
    return NativeCallbackFFI<Return_T(*)(Arg_Ts...)>::callRuby(indices, args...);
  }
}
#endif // HAVE_LIBFFI
