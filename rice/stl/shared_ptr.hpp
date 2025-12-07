#ifndef Rice__stl__shared_ptr__hpp_
#define Rice__stl__shared_ptr__hpp_

RICE_DETAIL_BEGIN_NAMESPACE
  template<typename T>
  class Wrapper<std::shared_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(const std::shared_ptr<T>& data);
    ~Wrapper();
    void* get() override;
    std::shared_ptr<T>& data();

  private:
    std::shared_ptr<T> data_;
  };
RICE_DETAIL_END_NAMESPACE

RICE_BEGIN_NAMESPACE
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName = "");
RICE_END_NAMESPACE

#include "shared_ptr.ipp"

#endif // Rice__stl__shared_ptr__hpp_
