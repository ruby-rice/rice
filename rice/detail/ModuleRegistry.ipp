#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeindex>


RICE_DETAIL_BEGIN_NAMESPACE
  inline void ModuleRegistry::add(VALUE module)
  {
    this->modules_.insert(module);
  }

  inline VALUE ModuleRegistry::modules()
  {
    Array result;

    for (const VALUE& value : this->modules_)
    {
      Module module(value);
      result.push(module, false);
    }
    return result;
  }
RICE_DETAIL_END_NAMESPACE
