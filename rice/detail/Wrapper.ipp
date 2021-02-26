#include <memory>

namespace Rice
{

namespace detail
{

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
class WrapperOwner : public Wrapper<T>
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

  T* get() override
  {
    return this->data_;
  }

private:
  T* data_;
};

template <typename T>
class WrapperReference : public Wrapper<T>
{
public:
  WrapperReference(const T& data): data_(data)
  {
  }

  T* get() override
  {
    return (T*)&this->data_;
  }

private:
  const T& data_;
};

template <typename T>
class WrapperPointer : public Wrapper<T>
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

  T* get() override
  {
    return this->data_;
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
inline void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool takeOwnership)
{
  WrapperPointer<T>* wrapper = nullptr;
  TypedData_Get_Struct(value, WrapperPointer<T>, rb_type, wrapper);
  delete wrapper;

  RTYPEDDATA_DATA(value) = new WrapperPointer<T>(data, takeOwnership);
}

template <typename T>
inline Wrapper<T>* getWrapper(VALUE value)
{
  return (Wrapper<T>*)RTYPEDDATA_DATA(value);
}

} // namespace
} // namespace
