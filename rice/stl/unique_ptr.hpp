#ifndef Rice__stl__unique_ptr__hpp_
#define Rice__stl__unique_ptr__hpp_

namespace Rice
{
  template<typename T>
  Data_Type<std::unique_ptr<T>> define_unique_ptr(std::string klassName = "");
}

namespace Rice::detail
{
  template<typename T>
  class Wrapper<std::unique_ptr<T>> : public WrapperBase
  {
  public:
    Wrapper(rb_data_type_t* rb_data_type, std::unique_ptr<T>&& data);
    ~Wrapper();
    void* get(rb_data_type_t* requestedType) override;

  private:
    std::unique_ptr<T> data_;
    rb_data_type_t* inner_rb_data_type_;
  };
}

#include "unique_ptr.ipp"

#endif // Rice__stl__unique_ptr__hpp_