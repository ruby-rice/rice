#ifndef Rice__stl__map__hpp_
#define Rice__stl__map__hpp_

RICE_BEGIN_NAMESPACE
  template<typename K, typename T>
  Data_Type<std::map<K, T>> define_map(std::string name = "");
RICE_END_NAMESPACE

#include "map.ipp"

#endif // Rice__stl__map__hpp_
