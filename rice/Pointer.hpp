#ifndef Rice__Pointer__hpp_
#define Rice__Pointer__hpp_

RICE_BEGIN_NAMESPACE
  template<typename T>
  class Pointer
  {
  };

  template<typename T>
  Data_Type<Pointer<T>> define_pointer(std::string klassName = "");
RICE_END_NAMESPACE

#endif // Rice__Pointer__hpp_
