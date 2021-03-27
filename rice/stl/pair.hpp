#ifndef Rice__stl__pair__hpp_
#define Rice__stl__pair__hpp_

#include "../detail/ruby.hpp"

namespace Rice
{
  template<typename T>
  Data_Type<T> define_pair(std::string name);

  template<typename T>
  Data_Type<T> define_pair_under(Object module, std::string name);
}

#include "pair.ipp"

#endif // Rice__stl__pair__hpp_

