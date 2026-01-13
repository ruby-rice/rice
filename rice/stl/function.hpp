#ifndef Rice__stl__function__hpp_
#define Rice__stl__function__hpp_

namespace Rice
{
  template<typename Signature_T>
  Data_Type<std::function<Signature_T>> define_stl_function(std::string klassName = "");
}

#include "function.ipp"

#endif // Rice__stl__function__hpp_
