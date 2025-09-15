#ifndef Rice__Registries__hpp_
#define Rice__Registries__hpp_

namespace Rice::detail
{
  class Registries
  {
  public:
    static Registries instance;

  public:
    HandlerRegistry handlers;
    InstanceRegistry instances;
    ModuleRegistry modules;
    NativeRegistry natives;
    TypeRegistry types;
  };
}

#endif // Rice__Registries__hpp_
