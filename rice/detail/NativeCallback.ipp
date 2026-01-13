namespace Rice::detail
{
#ifdef HAVE_LIBFFI
  template<typename Return_T, typename ...Parameter_Ts>
  template<typename Arg_T>
  ffi_type* NativeCallback<Return_T(*)(Parameter_Ts...)>::ffiType()
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
    
    if constexpr (std::is_pointer_v<Arg_T> || std::is_reference_v<Arg_T>)
    {
      return &ffi_type_pointer;
    }
    else
    {
      const std::type_index& key = std::type_index(typeid(Arg_T));
      return nativeToFfiMapping[key];
    }
  }

  // This is the function provided to the C++ callback with FFI
  template<typename Return_T, typename ...Parameter_Ts>
  void NativeCallback<Return_T(*)(Parameter_Ts...)>::invokeFFI(ffi_cif*, void* ret, void* args[], void* instance)
  {
    NativeCallback_T* self = (NativeCallback_T*)instance;

    // Define a helper lambda to invoke callRuby with unpacked args
    auto indices = std::make_index_sequence<sizeof...(Parameter_Ts)>{};
    auto helper = [&](auto&&... args)
    {
      if constexpr (!std::is_void_v<Return_T>)
      {
        *(Return_T*)ret = self->callRuby(indices, std::forward<Parameter_Ts>(args)...);
      }
      else
      {
        self->callRuby(indices, std::forward<Parameter_Ts>(args)...);
      }
    };

    // Convert the C style array to a tuple
    Tuple_T argsTuple = convertArgsToTuple(args, indices);

    // Now use std::apply to unpack the tuple and call the lamba helper
    std::apply(helper, std::forward<Tuple_T>(argsTuple));
  }
#endif //HAVE_LIBFFI

  // This is the function provided to the C++ callback without FFI
  template<typename Return_T, typename ...Parameter_Ts>
  inline Return_T NativeCallback<Return_T(*)(Parameter_Ts...)>::invoke(Parameter_Ts...args)
  {
    auto indices = std::make_index_sequence<sizeof...(Parameter_Ts)>{};

    if constexpr (std::is_void_v<Return_T>)
    {
      NativeCallback::native_->callRuby(indices, args...);
    }
    else
    {
      return NativeCallback::native_->callRuby(indices, args...);
    }
  }

  template<typename Return_T, typename ...Parameter_Ts>
  std::vector<std::unique_ptr<ParameterAbstract>> NativeCallback<Return_T(*)(Parameter_Ts...)>::copyParameters()
  {
    std::vector<std::unique_ptr<ParameterAbstract>> result;

    if (sizeof...(Parameter_Ts) > 0 && parameters_.empty())
    {
      NativeCallback_T::define();
    }

    if (parameters_.size() > 0)
    {
      auto indices = std::make_index_sequence<sizeof...(Parameter_Ts)>{};
      copyParametersImpl(result, indices);
    }
    return result;
  }

  template<typename Return_T, typename ...Parameter_Ts>
  template<std::size_t... I>
  void NativeCallback<Return_T(*)(Parameter_Ts...)>::copyParametersImpl(std::vector<std::unique_ptr<ParameterAbstract>>& result, std::index_sequence<I...>)
  {
    (result.push_back(std::make_unique<Parameter<Parameter_Ts>>(*(Parameter<Parameter_Ts>*)parameters_[I].get())), ...);
  }

  template<typename Return_T, typename ...Parameter_Ts>
  std::unique_ptr<Return> NativeCallback<Return_T(*)(Parameter_Ts...)>::copyReturnInfo()
  {
    if (!returnInfo_)
    {
      NativeCallback_T::define();
    }

    return std::make_unique<Return>(*returnInfo_);
  }

  template<typename Return_T, typename ...Parameter_Ts>
  template<typename Parameter_T>
  Parameter_T NativeCallback<Return_T(*)(Parameter_Ts...)>::extractArg(void* arg)
  {
    if constexpr (std::is_reference_v<Parameter_T>)
    {
      // We told libffi to pass references as pointers, so arg points to the pointer
      return static_cast<Parameter_T>(**reinterpret_cast<std::remove_reference_t<Parameter_T>**>(arg));
    }
    else
    {
      return *reinterpret_cast<Parameter_T*>(arg);
    }
  }

  template<typename Return_T, typename ...Parameter_Ts>
  template<std::size_t... I>
  typename NativeCallback<Return_T(*)(Parameter_Ts...)>::Tuple_T NativeCallback<Return_T(*)(Parameter_Ts...)>::convertArgsToTuple(void* args[], std::index_sequence<I...>&)
  {
    /* Loop over each value returned from Ruby and convert it to the appropriate C++ type based
       on the arguments (Parameter_Ts) required by the C++ function. Arg_T may have const/volatile while
       the associated From_Ruby<T> template parameter will not. Thus From_Ruby produces non-const values
       which we let the compiler convert to const values as needed. This works except for
       T** -> const T**, see comment in convertToNative method. */
    return std::forward_as_tuple(extractArg<Parameter_Ts>(args[I])...);
  }

  template<typename Return_T, typename ...Parameter_Ts>
  VALUE NativeCallback<Return_T(*)(Parameter_Ts...)>::finalizerCallback(VALUE, VALUE callback_arg, int, const VALUE*, VALUE)
  {
    NativeCallback_T* nativeCallback = (NativeCallback_T*)callback_arg;
    delete nativeCallback;
    return Qnil;
  }

  template<typename Return_T, typename...Parameter_Ts>
  template<typename ...Arg_Ts>
  inline void NativeCallback<Return_T(*)(Parameter_Ts...)>::define(Arg_Ts&& ...args)
  {
    // Create parameters and save them in static member so we can use them later when constructing an instance
    using Parameter_Tuple = std::tuple<Parameter_Ts...>;
    NativeCallback_T::parameters_ = Native::create_parameters<Parameter_Tuple>(std::forward<Arg_Ts>(args)...);

    // Create Return and save it in static member so we can use them later when constructing an instance
    NativeCallback_T::returnInfo_ = Native::create_return<Arg_Ts...>(args...);
  }

  template<typename Return_T, typename ...Parameter_Ts>
  NativeCallback<Return_T(*)(Parameter_Ts...)>::NativeCallback(VALUE proc) :
    Native("callback", copyReturnInfo(), copyParameters()),
      proc_(proc), fromRuby_(returnInfo_.get())
  {
    // Tie the lifetime of the NativeCallback C++ instance to the lifetime of the Ruby proc object
    VALUE finalizer = rb_proc_new(NativeCallback_T::finalizerCallback, (VALUE)this);
    rb_define_finalizer(proc, finalizer);

#ifdef HAVE_LIBFFI
    // First setup description of callback
    if (cif_.bytes == 0)
    {
      ffi_prep_cif(&cif_, FFI_DEFAULT_ABI, sizeof...(Parameter_Ts), ffiType<Return_T>(), args_.data());
    }

    // Now allocate memory
    this->closure_ = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure) + sizeof(void*), (void**)(&this->callback_));
    // Now create the closure which will create a C++ callback
    ffi_prep_closure_loc(this->closure_, &cif_, invokeFFI, (void*)this, (void*)this->callback_);
#else
    NativeCallback_T::callback_ = &NativeCallback_T::invoke;
    NativeCallback_T::native_ = this;
#endif
  }

  template<typename Return_T, typename ...Parameter_Ts>
  NativeCallback<Return_T(*)(Parameter_Ts...)>::~NativeCallback()
  {
#ifdef HAVE_LIBFFI
    ffi_closure_free(this->closure_);
#endif

    NativeCallback_T::callback_ = nullptr;
    NativeCallback_T::native_ = nullptr;

    this->proc_ = Qnil;
  }

  template<typename Return_T, typename ...Parameter_Ts>
  typename NativeCallback<Return_T(*)(Parameter_Ts...)>::Callback_T NativeCallback<Return_T(*)(Parameter_Ts...)>::callback()
  {
    return this->callback_;
  }

  template<typename Return_T, typename ...Parameter_Ts>
  template<std::size_t... I>
  Return_T NativeCallback<Return_T(*)(Parameter_Ts...)>::callRuby(std::index_sequence<I...>&, Parameter_Ts...args)
  {
    // Convert the C++ arguments to Ruby VALUEs
    std::array<VALUE, sizeof...(Parameter_Ts)> values = {
      dynamic_cast<Parameter<Parameter_Ts>*>(this->parameters_[I].get())->
               convertToRuby(args)... };

    static Identifier id("call");
    VALUE result = detail::protect(rb_funcallv, this->proc_, id.id(), (int)sizeof...(Parameter_Ts), values.data());
    if constexpr (!std::is_void_v<Return_T>)
    {
      return this->fromRuby_.convert(result);
    }
  }

  template<typename Return_T, typename ...Parameter_Ts>
  inline VALUE NativeCallback<Return_T(*)(Parameter_Ts...)>::operator()(std::map<std::string, VALUE>&, VALUE)
  {
    return Qnil;
  }

  template<typename Return_T, typename ...Parameter_Ts>
  inline std::string NativeCallback<Return_T(*)(Parameter_Ts...)>::toString()
  {
    return "";
  }

  //NativeKind kind() override;
  template<typename Return_T, typename ...Parameter_Ts>
  inline NativeKind NativeCallback<Return_T(*)(Parameter_Ts...)>::kind()
  {
    return NativeKind::Callback;
  }

  template<typename Return_T, typename ...Parameter_Ts>
  inline VALUE NativeCallback<Return_T(*)(Parameter_Ts...)>::returnKlass()
  {
    // Check if an array is being returned
    bool isBuffer = dynamic_cast<ReturnBuffer*>(this->returnInfo_.get()) ? true : false;
    if (isBuffer)
    {
      TypeDetail<Pointer<detail::remove_cv_recursive_t<std::remove_pointer_t<Return_T>>>> typeDetail;
      return typeDetail.rubyKlass();
    }
    else
    {
      TypeDetail<Return_T> typeDetail;
      return typeDetail.rubyKlass();
    }
  }
}
