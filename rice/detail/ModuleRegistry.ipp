#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeindex>


namespace Rice::detail
{
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
}