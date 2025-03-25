#ifndef Rice__stl__shared_ptr__hpp_
#define Rice__stl__shared_ptr__hpp_

namespace Rice::detail
{
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
}

namespace Rice
{
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName = "");
}

#include "shared_ptr.ipp"

#endif // Rice__stl__shared_ptr__hpp_