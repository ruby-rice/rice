#include <memory>

namespace Rice
{

namespace detail
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
  template <typename U = T>
  WrapperOwner(U&& data)
  {
    this->data_ = new T(std::forward<U>(data));
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
  WrapperPointer(T* data, bool takeOwnership) :
    data_(data), isOwner_(takeOwnership)
  {
  }

  ~WrapperPointer()
  {
    if (this->isOwner_)
    {
      delete this->data_;
    }
  }

  void replace(T* data, bool takeOwnership)
  {
    if (this->isOwner_)
    {
      delete this->data_;
    }
    this->data_ = data;
    this->isOwner_ = takeOwnership;
  }

  void* get() override
  {
    return (void*)this->data_;
  }

private:
  T* data_ = nullptr;
  bool isOwner_;
};


// ---- Helper Functions -------
template <typename T>
inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T&& data, bool takeOwnership)
{
  using Base_T = std::remove_reference_t<T>;
  if (takeOwnership)
  {
    WrapperOwner<Base_T>* wrapper = new WrapperOwner<Base_T>(data);
    return TypedData_Wrap_Struct(klass, rb_type, wrapper);
  }
  else
  {
    WrapperReference<Base_T>* wrapper = new WrapperReference<Base_T>(data);
    return TypedData_Wrap_Struct(klass, rb_type, wrapper);
  }
};

template <typename T>
inline VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool takeOwnership)
{
  WrapperPointer<T>* wrapper = new WrapperPointer<T>(data, takeOwnership);
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
inline void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool takeOwnership)
{
  WrapperPointer<T>* wrapper = nullptr;
  TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
  delete wrapper;

  RTYPEDDATA_DATA(value) = new WrapperPointer<T>(data, takeOwnership);
}

inline Wrapper* getWrapper(VALUE value)
{
  return static_cast<Wrapper*>(RTYPEDDATA_DATA(value));
}

} // namespace
} // namespace
