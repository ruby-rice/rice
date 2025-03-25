#ifndef Rice__stl__unordered_map__hpp_
#define Rice__stl__unordered_map__hpp_

namespace Rice
{
  template<typename Key, typename T>
  Data_Type<std::unordered_map<Key, T>> define_unordered_map(std::string name = "");
}

#include "unordered_map.ipp"

#endif // Rice__stl__unordered_map__hpp_