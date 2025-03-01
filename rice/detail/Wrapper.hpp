#ifndef Rice__detail__Wrapper__hpp_
#define Rice__detail__Wrapper__hpp_

namespace Rice::detail
{
  class WrapperBase
  {
  public:
    WrapperBase(bool isOwner = false);
    virtual ~WrapperBase() = default;
    virtual void* get() = 0;

    void ruby_mark();
    void addKeepAlive(VALUE value);
    void setOwner(bool value);

  protected:
    bool isOwner_ = false;

  private:
    // We use a vector for speed and memory locality versus a set which does
    // not scale well when getting to tens of thousands of objects (not expecting
    // that to happen...but just in case)
    std::vector<VALUE> keepAlive_;
  };

  template <typename T, typename Wrapper_T = void>
  VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T& data, bool isOwner);

  template <typename T, typename Wrapper_T = void>
  VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool isOwner);

  template <typename T>
  T* unwrap(VALUE value, rb_data_type_t* rb_type, bool takeOwnership);

  WrapperBase* getWrapper(VALUE value, rb_data_type_t* rb_type);

  template <typename T>
  void wrapConstructed(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner);

  WrapperBase* getWrapper(VALUE value);
}
#endif // Rice__detail__Wrapper__hpp_
 