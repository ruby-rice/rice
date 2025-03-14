#ifndef Rice__stl__pair__hpp_
#define Rice__stl__pair__hpp_

namespace Rice
{
  template<typename T>
  Data_Type<T> define_pair(std::string name);

  template<typename T>
  Data_Type<T> define_pair_under(Object parent, std::string name);
}

#include "pair.ipp"

#endif // Rice__stl__pair__hpp_

