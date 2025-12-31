#ifndef Rice__detail__Forwards__hpp_
#define Rice__detail__Forwards__hpp_

namespace Rice::detail
{
  // Setup method forwarding from a wrapper class to its wrapped type using Ruby's Forwardable.
  // This allows calling methods on the wrapper that get delegated to the wrapped object via
  // a "get" method that returns the wrapped object.
  //
  // Parameters:
  //   wrapper_klass - The Ruby class to add forwarding to (e.g., SharedPtr_MyClass)
  //   wrapped_klass - The Ruby class whose methods should be forwarded (e.g., MyClass)
  void define_forwarding(VALUE wrapper_klass, VALUE wrapped_klass);
}

#include "Forwards.ipp"

#endif // Rice__detail__Forwards__hpp_
