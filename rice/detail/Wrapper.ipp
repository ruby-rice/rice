#include <memory>

namespace Rice::detail
{
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

  template <typename T>
  class WrapperValue : public Wrapper
  {
  public:
    WrapperValue(T& data): data_(std::move(data))
    {
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
    WrapperReference(const T& data): data_(data)
    {
    }

    void* get() override
    {
      return (void*)&this->data_;
    }

  private:
    const T& data_;
  };

  template <typename T>
  class WrapperPointer : public Wrapper
  {
  public:
    WrapperPointer(T* data, bool isOwner) : data_(data), isOwner_(isOwner)
    {
    }

    ~WrapperPointer()
    {
      if (this->isOwner_)
      {
        delete this->data_;
      }
    }

    void* get() override
    {
      return (void*)this->data_;
    }

  private:
    T* data_ = nullptr;
    bool isOwner_ = false;
  };


  // ---- Helper Functions -------
  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T& data, bool isOwner)
  {
    if (isOwner)
    {
      WrapperValue<T>* wrapper = new WrapperValue<T>(data);
      return TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    else
    {
      WrapperReference<T>* wrapper = new WrapperReference<T>(data);
      return TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
  };

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    WrapperPointer<T>* wrapper = new WrapperPointer<T>(data, isOwner);
    return TypedData_Wrap_Struct(klass, rb_type, wrapper);
  };

  template <typename T>
  inline T* unwrap(VALUE value, rb_data_type_t* rb_type)
  {
    Wrapper* wrapper = nullptr;
    TypedData_Get_Struct(value, Wrapper, rb_type, wrapper);
    void* data = wrapper->get();
    return static_cast<T*>(data);
  }

  inline void* unwrap(VALUE value)
  {
    // Direct access to avoid any type checking
    Wrapper* wrapper = (Wrapper*)RTYPEDDATA_DATA(value);
    return wrapper->get();
  }

  template <typename T>
  inline void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner)
  {
    WrapperPointer<T>* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
    delete wrapper;

    wrapper = new WrapperPointer<T>(data, true);
    RTYPEDDATA_DATA(value) = wrapper;
  }

  inline Wrapper* getWrapper(VALUE value)
  {
    return static_cast<Wrapper*>(RTYPEDDATA_DATA(value));
  }
} // namespace
