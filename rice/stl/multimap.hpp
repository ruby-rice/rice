#ifndef Rice__stl__multimap__hpp_
#define Rice__stl__multimap__hpp_

#include <map>

RICE_BEGIN_NAMESPACE
  template<typename K, typename T>
  Data_Type<std::multimap<K, T>> define_multimap(std::string name = "");
RICE_END_NAMESPACE

#include "multimap.ipp"

#endif // Rice__stl__multimap__hpp_
