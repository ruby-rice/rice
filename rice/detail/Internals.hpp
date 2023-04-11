#ifndef Rice__Internals__hpp_
#define Rice__Internals__hpp_

#include "TypeRegistry.hpp"
#include "InstanceTracker.hpp"

namespace Rice::detail
{
  class Internals
  {
  public:
    static Internals instance;

  public:
    TypeRegistry typeRegistry;
    InstanceTracker instanceTracker;
  };
}

#include "Internals.ipp"

#endif // Rice__Internals__hpp_
