#ifndef Rice__stl__smart_ptr__hpp_
#define Rice__stl__smart_ptr__hpp_

#include "../detail/Wrapper.hpp"

namespace Rice::detail
{
  template <template <typename, typename...> typename SmartPointer_T, typename...Arg_Ts>
  class WrapperSmartPointer : public Wrapper
  {
  public:
    WrapperSmartPointer(SmartPointer_T<Arg_Ts...>& data);
    void* get() override;
    SmartPointer_T<Arg_Ts...>& data();

  private:
    SmartPointer_T<Arg_Ts...> data_;
  };
}

#include "smart_ptr.ipp"

#endif // Rice__stl__smart_ptr__hpp_