#ifndef Rice__stl__multimap__hpp_
#define Rice__stl__multimap__hpp_

#include <map>

namespace Rice
{
  template<typename K, typename T>
  Data_Type<std::multimap<K, T>> define_multimap(std::string name = "");
}

#include "multimap.ipp"

#endif // Rice__stl__multimap__hpp_