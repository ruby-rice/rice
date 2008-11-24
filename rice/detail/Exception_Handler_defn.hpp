#ifndef Rice__detail__Exception_Handler_defn__hpp_
#define Rice__detail__Exception_Handler_defn__hpp_

#include "ruby.h"
#undef TYPE

#include "Not_Copyable.hpp"
#include "../Address_Registration_Guard_defn.hpp"
#include "../Data_Object_defn.hpp"

namespace Rice
{

namespace detail
{

// A class for converting C++ exceptions to ruby exceptions.  It's used
// like this:
//
//   try
//   {
//   }
//   catch(...)
//   {
//     handler->handle_exception();
//   }
//
// If an exception is thrown the handler will pass the exception up the
// chain, then the last handler in the chain will throw the exception
// down the chain until a lower handler can handle it, e.g.:
//
//   try
//   {
//     return call_next_exception_handler();
//   }
//   catch(MyException const & ex)
//   {
//     throw Rice::Exception(rb_cMyException, "%s", ex.what());
//   }
//
class Exception_Handler
  : public Rice::detail::Not_Copyable
{
public:
  Exception_Handler(
      Data_Object<Exception_Handler> next_exception_handler);

  virtual ~Exception_Handler();

  virtual VALUE handle_exception() const = 0;

  VALUE call_next_exception_handler() const;

private:
  Data_Object<Exception_Handler> next_exception_handler_;
  Address_Registration_Guard next_exception_handler_guard_;
};

// The default exception handler just rethrows the exception.  If there
// are other handlers in the chain, they will try to handle the rethrown
// exception.
class Default_Exception_Handler
  : public Exception_Handler
{
public:
  Default_Exception_Handler();

  virtual VALUE handle_exception() const;
};

// An exception handler that takes a functor as an argument.  The
// functor should throw a Rice::Exception to handle the exception.  If
// the functor does not handle the exception, the exception will be
// re-thrown.
template <typename Exception_T, typename Functor_T>
class Functor_Exception_Handler
  : public Exception_Handler
{
public:
  Functor_Exception_Handler(
      Functor_T handler,
      Data_Object<Exception_Handler> next_exception_handler);

private:
  virtual VALUE handle_exception() const;

private:
  Functor_T handler_;
};

} // namespace detail

} // namespace Rice

#endif // Rice__detail__Exception_Handler_defn__hpp_

