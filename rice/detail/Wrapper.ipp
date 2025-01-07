#include <memory>

namespace Rice::detail
{
  inline Wrapper::Wrapper(bool isOwner): isOwner_(isOwner)
  {
  }

  inline void Wrapper::ruby_mark()
  {
    for (VALUE value : this->keepAlive_)
    {
      rb_gc_mark(value);
    }
  }

  inline void Wrapper::addKeepAlive(VALUE value)
  {
    this->keepAlive_.push_back(value);
  }

  inline void Wrapper::setOwner(bool value)
  {
    this->isOwner_ = value;
  }

  template <typename T>
  class WrapperValue : public Wrapper
  {
  public:
    WrapperValue(T& data): data_(std::move(data))
    {
    }

    ~WrapperValue()
    {
      Registries::instance.instances.remove(this->get());
    }

    void* get() override
    {
      return (void*)&this->data_;
    }

  private:
    T data_;
  };

  template <typename T>
  class WrapperReference : public Wrapper
  {
  public:
    WrapperReference(T& data): data_(data)
    {
    }

    ~WrapperReference()
    {
      Registries::instance.instances.remove(this->get());
    }

    void* get() override
    {
      return (void*)&this->data_;
    }

  private:
    T& data_;
  };

  template <typename T>
  class WrapperPointer : public Wrapper
  {
  public:
    WrapperPointer(T* data, bool isOwner) : Wrapper(isOwner), data_(data)
    {
    }

    ~WrapperPointer()
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

    void* get() override
    {
      return (void*)this->data_;
    }

  private:
    T* data_ = nullptr;
  };

  // ---- Helper Functions -------
  template <typename T, typename Wrapper_T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T& data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(&data);

    if (result != Qnil)
      return result;

    Wrapper* wrapper = nullptr;

    // Is this a pointer but cannot be copied? For example a std::unique_ptr
    if constexpr (!std::is_void_v<Wrapper_T> && !std::is_copy_constructible_v<Wrapper_T>)
    {
      wrapper = new Wrapper_T(std::move(data));
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    // Is this a pointer or smart pointer like std::shared_ptr
    else if constexpr (!std::is_void_v<Wrapper_T>)
    {
      wrapper = new Wrapper_T(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    // Is this a pointer and it cannot copied? This is for std::unique_ptr
    // If ruby is the owner than copy the object
    else if (isOwner)
    {
      wrapper = new WrapperValue<T>(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    // Ruby is not the owner so just wrap the reference
    else
    {
      wrapper = new WrapperReference<T>(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }

    Registries::instance.instances.add(wrapper->get(), result);

    return result;
  };

  template <typename T, typename Wrapper_T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    VALUE result = Registries::instance.instances.lookup(data);

    if (result != Qnil)
      return result;

    Wrapper* wrapper = nullptr;

    if constexpr (!std::is_void_v<Wrapper_T>)
    {
      wrapper = new Wrapper_T(data);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    else
    {
      wrapper = new WrapperPointer<T>(data, isOwner);
      result = TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }

    Registries::instance.instances.add(wrapper->get(), result);
    return result;
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_type, bool transferOwnership)
  {
    Wrapper* wrapper = getWrapper(value, rb_type);
    if (transferOwnership)
      wrapper->setOwner(false);

    if (wrapper == nullptr)
    {
      std::string message = "Wrapped C++ object is nil. Did you override " + 
                            std::string(detail::protect(rb_obj_classname, value)) + 
                            "#initialize and forget to call super?";

      throw std::runtime_error(message);
    }

    return static_cast<T*>(wrapper->get());
  }
    
  inline Wrapper* getWrapper(VALUE value, rb_data_type_t* rb_type)
  {
    Wrapper* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper, rb_type, wrapper);
    return wrapper;
  }

  inline Wrapper* getWrapper(VALUE value)
  {
    // Turn off spurious warning on g++ 12
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
    #endif

    return RTYPEDDATA_P(value) ? static_cast<Wrapper*>(RTYPEDDATA_DATA(value)) : nullptr;
    
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
  }

  template <typename T>
  inline void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    WrapperPointer<T>* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
    if (wrapper)
    {
      Registries::instance.instances.remove(wrapper->get());
      delete wrapper;
    }

    wrapper = new WrapperPointer<T>(data, isOwner);
    RTYPEDDATA_DATA(value) = wrapper;

    Registries::instance.instances.add(data, value);
  }
} // namespace
