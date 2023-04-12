#ifndef Rice__Registries__hpp_
#define Rice__Registries__hpp_

#include "HandlerRegistry.hpp"
#include "InstanceRegistry.hpp"
#include "TypeRegistry.hpp"

namespace Rice::detail
{
  class Registries
  {
  public:
    static Registries instance;

  public:
    HandlerRegistry handlers;
    InstanceRegistry instances;
    TypeRegistry types;
  };
}

#include "Registries.ipp"

#endif // Rice__Registries__hpp_
