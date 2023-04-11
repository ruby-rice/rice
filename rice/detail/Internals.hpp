#ifndef Rice__Internals__hpp_
#define Rice__Internals__hpp_

#include "HandlerRegistry.hpp"
#include "InstanceRegistry.hpp"
#include "TypeRegistry.hpp"

namespace Rice::detail
{
  class Internals
  {
  public:
    static Internals instance;

  public:
    HandlerRegistry handlerRegistry;
    InstanceRegistry instanceRegistry;
    TypeRegistry typeRegistry;
  };
}

#include "Internals.ipp"

#endif // Rice__Internals__hpp_
