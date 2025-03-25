#ifndef Rice__stl__vector__hpp_
#define Rice__stl__vector__hpp_

namespace Rice
{
  template<typename T>
  Data_Type<std::vector<T>> define_vector(std::string name = "" );
}

#include "vector.ipp"

#endif // Rice__stl__vector__hpp_