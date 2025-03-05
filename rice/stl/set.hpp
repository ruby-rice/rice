#ifndef Rice__stl__set__hpp_
#define Rice__stl__set__hpp_

#include "../detail/ruby.hpp"

namespace Rice
{
  template<typename T>
  Data_Type<T> define_set(std::string klassName = "");
}

#include "set.ipp"

#endif // Rice__stl__set__hpp_