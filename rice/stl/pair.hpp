#ifndef Rice__stl__pair__hpp_
#define Rice__stl__pair__hpp_

namespace Rice
{
  template<typename T1, typename T2>
  Data_Type<std::pair<T1, T2>> define_pair(std::string klassName = "");
}

#include "pair.ipp"

#endif // Rice__stl__pair__hpp_

