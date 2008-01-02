#include "../Data_Object.hpp"

template<typename T>
VALUE Rice::detail::
default_allocation_func(VALUE /* klass */)
{
  Data_Object<T> m(static_cast<T*>(0));
  return m.value();
} 


