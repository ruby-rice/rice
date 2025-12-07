#ifndef Rice__stl__vector__hpp_
#define Rice__stl__vector__hpp_

RICE_BEGIN_NAMESPACE
  template<typename T>
  Data_Type<std::vector<T>> define_vector(std::string name = "" );
RICE_END_NAMESPACE

#include "vector.ipp"

#endif // Rice__stl__vector__hpp_
