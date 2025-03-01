#ifndef Rice__stl__shared_ptr__hpp_
#define Rice__stl__shared_ptr__hpp_

namespace Rice::detail
{
  template<typename T>
  class Wrapper<std::shared_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(std::shared_ptr<T>& data);
    ~Wrapper();
    void* get() override;
    std::shared_ptr<T>& data();

  private:
    std::shared_ptr<T> data_;
  };
}

#include "shared_ptr.ipp"

#endif // Rice__stl__shared_ptr__hpp_