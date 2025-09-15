#ifndef Rice__Pointer__hpp_
#define Rice__Pointer__hpp_

namespace Rice
{
  template<typename T>
  class Pointer
  {
  };

  template<typename T>
  Data_Type<Pointer<T>> define_pointer(std::string klassName = "");
}

#endif // Rice__Pointer__hpp_
