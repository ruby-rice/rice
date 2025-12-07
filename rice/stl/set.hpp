#ifndef Rice__stl__set__hpp_
#define Rice__stl__set__hpp_

RICE_BEGIN_NAMESPACE
  template<typename T>
  Data_Type<std::set<T>> define_set(std::string klassName = "");
RICE_END_NAMESPACE

#include "set.ipp"

#endif // Rice__stl__set__hpp_
