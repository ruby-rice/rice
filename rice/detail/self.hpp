#ifndef Rice__detail__self__hpp_
#define Rice__detail__self__hpp_

namespace Rice::detail
{
  // Thread local variable that points to RUBY self value. Currently this is only needed
  // in the case of creating enumerators. Alternative ways of doing this are adding an extra
  // parameter to the method call but that adds a lot of complexity to this code. Or a global
  // registry using instance tracking, but that is a more heavyweight solution.
  thread_local static VALUE selfThread;
}
#endif // Rice__detail__self__hpp_
