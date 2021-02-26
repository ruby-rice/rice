#include <memory>

namespace Rice
{

namespace detail
{

template <typename T>
class WrapperOwner : public Wrapper<T>
{
public:
  template <typename U = T>
  WrapperOwner(U&& data);
  ~WrapperOwner();
  T* get() override;

private:
  T* data_;
};

template <typename T>
class WrapperReference : public Wrapper<T>
{
public:
  WrapperReference(const T& data);
  T* get() override;

private:
  const T& data_;
};

template <typename T>
class WrapperPointer : public Wrapper<T>
{
public:
  WrapperPointer(T* data, bool takeOwnership);
  ~WrapperPointer();
  void update(T* data, bool takeOwnership);
  T* get() override;

private:
  T* data_ = nullptr;
  bool isOwner_;
};

template <typename T>
inline void Wrapper<T>::ruby_mark()
{
  for (VALUE value : this->keepAlive_)
  {
    rb_gc_mark(value);
  }
}

template <typename T>
inline void Wrapper<T>::addKeepAlive(VALUE value)
{
  this->keepAlive_.push_back(value);
}

template <typename T>
template <typename U>
inline WrapperOwner<T>::WrapperOwner(U&& data)
{
  this->data_ = new T(std::forward<U>(data));
}

template <typename T>
inline WrapperOwner<T>::~WrapperOwner()
{
  delete this->data_;
}

template <typename T>
inline T* WrapperOwner<T>::get()
{
  return this->data_;
}

template <typename T>
inline WrapperReference<T>::WrapperReference(const T& data) : data_(data)
{
}

template <typename T>
T* WrapperReference<T>::get()
{
  return (T*)&this->data_;
}

template <typename T>
inline WrapperPointer<T>::WrapperPointer(T* data, bool takeOwnership) :
  data_(data), isOwner_(takeOwnership)
{
}

template <typename T>
inline WrapperPointer<T>::~WrapperPointer()
{
  if (this->isOwner_)
  {
    delete this->data_;
  }
}

template <typename T>
inline void WrapperPointer<T>::update(T* data, bool takeOwnership)
{
  if (this->isOwner_)
  {
    delete this->data_;
  }
  this->data_ = data;
  this->isOwner_ = takeOwnership;
}

template <typename T>
inline T* WrapperPointer<T>::get()
{
  return this->data_;
}

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
  Wrapper<T>* wrapper = nullptr;
  TypedData_Get_Struct(value, Wrapper<T>, rb_type, wrapper);
  return wrapper->get();
}

inline void* unwrap(VALUE value)
{
  Wrapper<void>* wrapper = (Wrapper<void>*)RTYPEDDATA_DATA(value);
  return wrapper->get();
}

template <typename T>
inline void update(VALUE value, rb_data_type_t* rb_type, T* data, bool takeOwnership)
{
  WrapperPointer<T>* wrapper = nullptr;
  TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
  wrapper->update(data, takeOwnership);
}

template <typename T>
inline Wrapper<T>* getWrapper(VALUE value)
{
  return (Wrapper<T>*)RTYPEDDATA_DATA(value);
}

} // namespace
} // namespace
