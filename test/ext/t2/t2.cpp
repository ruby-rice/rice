#include "rice/Data_Type.hpp"

using namespace Rice;

class Foo { };

extern "C"
void Init_t2()
{
  define_class<Foo>("Foo");
}

