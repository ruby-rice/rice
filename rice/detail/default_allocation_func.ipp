#include "../Data_Object.hpp"

template<typename T>
VALUE Rice::detail::
default_allocation_func(VALUE klass)
{
  // Create a new Ruby object but since we do not yet have a C++ object
  // just pass a nullptr. It will be set via the Constructor call
  return TypedData_Wrap_Struct(klass, Data_Type<T>::rb_type(), nullptr);
}