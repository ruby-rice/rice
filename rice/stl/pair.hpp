#ifndef Rice__stl__pair__hpp_
#define Rice__stl__pair__hpp_

RICE_BEGIN_NAMESPACE
  template<typename T1, typename T2>
  Data_Type<std::pair<T1, T2>> define_pair(std::string klassName = "");
RICE_END_NAMESPACE

#include "pair.ipp"

#endif // Rice__stl__pair__hpp_

