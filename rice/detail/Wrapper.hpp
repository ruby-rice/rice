#ifndef Rice__detail__Wrapper__hpp_
#define Rice__detail__Wrapper__hpp_

namespace Rice::detail
{
  class WrapperBase
  {
  public:
    WrapperBase() = default;
    virtual ~WrapperBase() = default;
    virtual void* get() = 0;
    bool isConst();

    void ruby_mark();
    void addKeepAlive(VALUE value);
    void setOwner(bool value);

  protected:
    bool isOwner_ = false;
    bool isConst_ = false;

  private:
    // We use a vector for speed and memory locality versus a set which does
    // not scale well when getting to tens of thousands of objects (not expecting
    // that to happen...but just in case)
    std::vector<VALUE> keepAlive_;
  };

  template <typename T>
  class Wrapper : public WrapperBase
  {
  public:
    Wrapper(T& data);
    Wrapper(T&& data);
    ~Wrapper();
    void* get() override;

  private:
    T data_;
  };

  template<typename T>
  class Wrapper<T&> : public WrapperBase
  {
  public:
    Wrapper(T& data);
    ~Wrapper();
    void* get() override;

  private:
    T& data_;
  };

  template <typename T>
  class Wrapper<T*> : public WrapperBase
  {
  public:
    Wrapper(T* data, bool isOwner);
    ~Wrapper();
    void* get() override;

  private:
    T* data_ = nullptr;
  };

  // ---- Helper Functions ---------
  template <typename T>
  void wrapConstructed(VALUE value, rb_data_type_t* rb_data_type, T* data, bool isOwner);

  template <typename T>
  VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T& data, bool isOwner);

  template <typename T>
  VALUE wrap(VALUE klass, rb_data_type_t* rb_data_type, T* data, bool isOwner);

  template <typename T>
  T* unwrap(VALUE value, rb_data_type_t* rb_data_type, bool takeOwnership);

  template <typename Wrapper_T = WrapperBase>
  Wrapper_T* getWrapper(VALUE value, rb_data_type_t* rb_data_type);

  WrapperBase* getWrapper(VALUE value);
}
#endif // Rice__detail__Wrapper__hpp_
 