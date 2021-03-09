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
  class WrapperOwner : public Wrapper
  {
  public:
    WrapperOwner(T& data)
    {
      // We own this object so we can call the move constructor
      this->data_ = new T(std::move(data));
    }

    ~WrapperOwner()
    {
      delete this->data_;
    }

    void* get() override
    {
      return (void*)this->data_;
    }

  private:
    T* data_;
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
    WrapperPointer(T* data) : data_(data)
    {
    }

    ~WrapperPointer()
    {
      if (this->isOwner)
      {
        delete this->data_;
      }
    }

    void replace(T* data)
    {
      if (this->isOwner)
      {
        delete this->data_;
      }
      this->data_ = data;
    }

    void* get() override
    {
      return (void*)this->data_;
    }

  private:
    T* data_ = nullptr;
  };


  // ---- Helper Functions -------
  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T&& data)
  {
    // This is confusing! If data is an rvalue, then T is not actually a reference type at all!
    if constexpr (!std::is_reference_v<T>)
    {
      // This is an rvalue so we are going to move construct it
      WrapperOwner<T>* wrapper = new WrapperOwner<T>(data);
      wrapper->isOwner = true;
      return TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
    else
    {
      // This is a lvalue and we are just going to store it
      using Intrinsic_T = std::remove_reference_t<T>;
      WrapperReference<Intrinsic_T>* wrapper = new WrapperReference<Intrinsic_T>(data);
      wrapper->isOwner = false;
      return TypedData_Wrap_Struct(klass, rb_type, wrapper);
    }
  };

  template <typename T>
  inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data)
  {
    WrapperPointer<T>* wrapper = new WrapperPointer<T>(data);
    wrapper->isOwner = false;
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
  inline void replace(VALUE value, rb_data_type_t* rb_type, T* data)
  {
    WrapperPointer<T>* wrapper = nullptr;
    TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
    delete wrapper;

    wrapper = new WrapperPointer<T>(data);
    wrapper->isOwner = true;
    RTYPEDDATA_DATA(value) = wrapper;
  }

  inline Wrapper* getWrapper(VALUE value)
  {
    return static_cast<Wrapper*>(RTYPEDDATA_DATA(value));
  }
} // namespace
