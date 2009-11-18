#include "../t1/Foo.hpp"
#include "rice/Data_Type.hpp"

using namespace Rice;

extern "C"
void Init_t2()
{
  volatile Data_Type<Foo> foo;
}

