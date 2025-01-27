#ifndef Rice__detail__Wrapper__hpp_
#define Rice__detail__Wrapper__hpp_

namespace Rice
{
namespace detail
{

class Wrapper
{
public:
  Wrapper(bool isOwner = false);
  virtual ~Wrapper() = default;
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

Wrapper* getWrapper(VALUE value, rb_data_type_t* rb_type);

template <typename T>
void replace(VALUE value, rb_data_type_t* rb_type, T* data, bool isOwner);

Wrapper* getWrapper(VALUE value);

} // namespace detail
} // namespace Rice

#endif // Rice__detail__Wrapper__hpp_

