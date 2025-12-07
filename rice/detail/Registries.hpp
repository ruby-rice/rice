#ifndef Rice__Registries__hpp_
#define Rice__Registries__hpp_

RICE_DETAIL_BEGIN_NAMESPACE
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
RICE_DETAIL_END_NAMESPACE

#endif // Rice__Registries__hpp_
