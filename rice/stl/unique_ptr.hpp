#ifndef Rice__stl__unique_ptr__hpp_
#define Rice__stl__unique_ptr__hpp_

RICE_DETAIL_BEGIN_NAMESPACE
  template<typename T>
  class Wrapper<std::unique_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(std::unique_ptr<T>&& data);
    ~Wrapper();
    void* get() override;
    std::unique_ptr<T>& data();

  private:
    std::unique_ptr<T> data_;
  };
RICE_DETAIL_END_NAMESPACE

#include "unique_ptr.ipp"

#endif // Rice__stl__unique_ptr__hpp_
