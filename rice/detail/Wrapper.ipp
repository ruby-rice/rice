#include <memory>

namespace Rice::detail
{

template <typename T>
class Wrapper
{
public:
  virtual ~Wrapper() = default;
  virtual T* get() = 0;
};

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

  void update(T* data, bool takeOwnership)
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
inline VALUE wrap(VALUE klass, RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free, T&& data, bool takeOwnership)
{
  using Base_T = std::remove_reference_t<T>;
  if (takeOwnership)
  {
    WrapperOwner<Base_T>* Wrapper = new WrapperOwner<Base_T>(data);
    return Data_Wrap_Struct(klass, mark, free, Wrapper);
  }
  else
  {
    WrapperReference<Base_T>* Wrapper = new WrapperReference<Base_T>(data);
    return Data_Wrap_Struct(klass, mark, free, Wrapper);
  }
};

template <typename T>
inline VALUE wrap(VALUE klass, RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free, T* data, bool takeOwnership)
{
  WrapperPointer<T>* Wrapper = new WrapperPointer<T>(data, takeOwnership);
  return Data_Wrap_Struct(klass, mark, free, Wrapper);
};

template <typename T>
inline T* unwrap(VALUE value)
{
  Wrapper<T>* wrapper = nullptr;
  Data_Get_Struct(value, Wrapper<T>, wrapper);
  return wrapper->get();
}

template <typename T>
inline void update(VALUE value, T* data, bool takeOwnership)
{
  WrapperPointer<T>* Wrapper = nullptr;
  Data_Get_Struct(value, WrapperPointer<T>, Wrapper);
  Wrapper->update(data, takeOwnership);
}

} // namespace
