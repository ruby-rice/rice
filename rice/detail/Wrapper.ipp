#include <memory>

RICE_DETAIL_BEGIN_NAMESPACE
  inline bool WrapperBase::isConst()
  {
    return this->isConst_;
  }

  inline void WrapperBase::ruby_mark()
  {
    for (VALUE value : this->keepAlive_)
    {
      rb_gc_mark(value);
    }
  }

  inline void WrapperBase::addKeepAlive(VALUE value)
  {
    this->keepAlive_.push_back(value);
  }

  inline void WrapperBase::setOwner(bool value)
  {
    this->isOwner_ = value;
  }

  // ----  Wrapper -----
  template <typename T>
  inline Wrapper<T>::Wrapper(T& data): data_(data)
  {
  }

  template <typename T>
  inline Wrapper<T>::Wrapper(T&& data) : data_(std::move(data))
  {
  }

  template <typename T>
  inline Wrapper<T>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <typename T>
  inline void* Wrapper<T>::Wrapper::get()
  {
    return (void*)&this->data_;
  }

  // ----  Wrapper& -----
  template <typename T>
  inline Wrapper<T&>::Wrapper(T& data): data_(data)
  {
    this->isConst_ = std::is_const_v<std::remove_reference_t<T>>;
  }

  template <typename T>
  inline Wrapper<T&>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
  }

  template <typename T>
  inline void* Wrapper<T&>::get()
  {
    return (void*)&this->data_;
  }

  // ----  Wrapper* -----
  template <typename T>
  inline Wrapper<T*>::Wrapper(T* data, bool isOwner) : data_(data)
  {
    this->isOwner_ = isOwner;
    this->isConst_ = std::is_const_v<std::remove_pointer_t<T>>;
  }

  template <typename T>
  inline Wrapper<T*>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
    if constexpr (std::is_destructible_v<T>)
    {
      if (this->isOwner_)
      {
        delete this->data_;
      }
    }
  }

  template <typename T>
  inline void* Wrapper<T*>::get()
  {
    return (void*)this->data_;
  }

  // ----  Wrapper** -----
  template <typename T>
  inline Wrapper<T**>::Wrapper(T** data, bool isOwner) : data_(data)
  {
    this->isOwner_ = isOwner;
    this->isConst_ = std::is_const_v<std::remove_pointer_t<std::remove_pointer_t<T>>>;
  }

  template <typename T>
  inline Wrapper<T**>::~Wrapper()
  {
    Registries::instance.instances.remove(this->get());
    if constexpr (std::is_destructible_v<T>)
    {
      if (this->isOwner_)
      {
        delete this->data_;
      }
    }
  }

  template <typename T>
  inline void* Wrapper<T**>::get()
  {
    return (void*)this->data_;
  }

  // ---- Helper Functions -------
  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T& data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(&data);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = nullptr;

    // If Ruby is not the owner then wrap the reference
    if (!isOwner)
    {
      wrapper = new Wrapper<T&>(data);
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
    }

    // std::is_copy_constructible_v<std::vector<std::unique_ptr<T>>>> returns true. Sigh.
    else if constexpr (detail::is_std_vector_v<T>)
    {
      if constexpr (std::is_copy_constructible_v<typename T::value_type>)
      {
        wrapper = new Wrapper<T>(data);
        result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
      }
      else
      {
        wrapper = new Wrapper<T>(std::move(data));
        result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
      }
    }

    // Ruby is the owner so copy data
    else if constexpr (std::is_copy_constructible_v<T>)
    {
      wrapper = new Wrapper<T>(data);
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
    }

    // Ruby is the owner so move data
    else if constexpr (std::is_move_constructible_v<T>)
    {
      wrapper = new Wrapper<T>(std::move(data));
      result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);
    }

    else
    {
      detail::TypeIndexParser typeIndexParser(typeid(T), std::is_fundamental_v<detail::intrinsic_type<T>>);
      std::string message = "Rice was directed to take ownership of a C++ object but it does not have an accessible copy or move constructor. Type: " +
        typeIndexParser.name();
      throw std::runtime_error(message);
    }

    Registries::instance.instances.add(wrapper->get(), result);

    return result;
  };

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T* data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(data);

    if (result != Qnil)
      return result;

    WrapperBase* wrapper = new Wrapper<T*>(data, isOwner);
    result = TypedData_Wrap_Struct(klass, rb_data_type, wrapper);

    Registries::instance.instances.add(wrapper->get(), result);
    return result;
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_data_type, bool takeOwnership)
  {
    if (rb_type(value) != RUBY_T_DATA)
    {
      std::string message = "The Ruby object does not wrap a C++ object. It is actually a " +
        std::string(detail::protect(rb_obj_classname, value)) + ".";
      throw std::runtime_error(message);
    }

    WrapperBase* wrapper = getWrapper(value, rb_data_type);

    if (wrapper == nullptr)
    {
      std::string message = "Wrapped C++ object is nil. Did you override " + 
                            std::string(detail::protect(rb_obj_classname, value)) + 
                            "#initialize and forget to call super?";

      throw std::runtime_error(message);
    }

    if (takeOwnership)
      wrapper->setOwner(false);

    return static_cast<T*>(wrapper->get());
  }
    
  template <typename Wrapper_T>
  inline Wrapper_T* getWrapper(VALUE value, rb_data_type_t* rb_data_type)
  {
    WrapperBase* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperBase, rb_data_type, wrapper);
    return dynamic_cast<Wrapper_T*>(wrapper);
  }

  inline WrapperBase* getWrapper(VALUE value)
  {
    // Turn off spurious warning on g++ 12
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

    if (!RTYPEDDATA_P(value))
    {
      throw Exception(rb_eTypeError, "wrong argument type %s (expected %s)",
        detail::protect(rb_obj_classname, value), "wrapped C++ object");
    }

    return static_cast<WrapperBase*>(RTYPEDDATA_DATA(value));
    
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
  }

  template <typename T>
  inline void wrapConstructed(VALUE value, rb_data_type_t* rb_data_type, T* data)
  {
    using Wrapper_T = Wrapper<T*>;
    
    Wrapper_T* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper_T, rb_data_type, wrapper);
    if (wrapper)
    {
      Registries::instance.instances.remove(wrapper->get());
      delete wrapper;
    }

    wrapper = new Wrapper_T(data, true);
    RTYPEDDATA_DATA(value) = wrapper;

    Registries::instance.instances.add(data, value);
  }
RICE_DETAIL_END_NAMESPACE
