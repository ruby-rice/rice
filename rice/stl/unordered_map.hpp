#ifndef Rice__stl__unordered_map__hpp_
#define Rice__stl__unordered_map__hpp_

#include "../detail/ruby.hpp"

namespace Rice
{
  template<typename U>
  Data_Type<U> define_unordered_map(std::string name);

  template<typename U>
  Data_Type<U> define_unordered_map_under(Object parent, std::string name);
}

#include "unordered_map.ipp"

#endif // Rice__stl__unordered_map__hpp_