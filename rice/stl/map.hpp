#ifndef Rice__stl__map__hpp_
#define Rice__stl__map__hpp_

namespace Rice
{
  template<typename U>
  Data_Type<U> define_map(std::string name);

  template<typename U>
  Data_Type<U> define_map_under(Object module, std::string name);
}

#include "map.ipp"

#endif // Rice__stl__map__hpp_