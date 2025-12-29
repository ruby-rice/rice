#ifndef Rice__stl__shared_ptr__hpp_
#define Rice__stl__shared_ptr__hpp_

namespace Rice
{
  template<typename T>
  Data_Type<std::shared_ptr<T>> define_shared_ptr(std::string klassName = "");
}

#include "shared_ptr.ipp"

#endif // Rice__stl__shared_ptr__hpp_