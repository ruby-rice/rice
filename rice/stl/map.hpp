#ifndef Rice__stl__map__hpp_
#define Rice__stl__map__hpp_

namespace Rice
{
  template<typename K, typename T>
  Data_Type<std::map<K, T>> define_map(std::string name = "");
}

#include "map.ipp"

#endif // Rice__stl__map__hpp_