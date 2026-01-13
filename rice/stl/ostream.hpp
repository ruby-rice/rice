#ifndef Rice__stl__ostream__hpp_
#define Rice__stl__ostream__hpp_

namespace Rice
{
  Data_Type<std::ostream> define_ostream();
  Data_Type<std::ostringstream> define_ostringstream(std::string klassName = "");
  Data_Type<std::ofstream> define_ofstream(std::string klassName = "");
}

#include "ostream.ipp"

#endif // Rice__stl__ostream__hpp_
