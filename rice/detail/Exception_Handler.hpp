#ifndef Rice__detail__Exception_Handler__hpp_
#define Rice__detail__Exception_Handler__hpp_

#include "ruby.h"
#undef TYPE

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
{
public:
  Exception_Handler(Exception_Handler const * next_exception_handler)
    : next_exception_handler_(next_exception_handler)
  {
  }

  virtual ~Exception_Handler()
  {
  }

  virtual VALUE handle_exception() const = 0;

  VALUE call_next_exception_handler() const
  {
    return next_exception_handler_->handle_exception();
  }

private:
  Exception_Handler const * next_exception_handler_;
};

// The default exception handler just rethrows the exception.  If there
// are other handlers in the chain, they will try to handle the rethrown
// exception.
class Default_Exception_Handler
  : public Exception_Handler
{
public:
  Default_Exception_Handler()
    : Exception_Handler(0)
  {
  }

  virtual VALUE handle_exception() const
  {
    throw;
  }
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
      Exception_Handler const * next_exception_handler)
    : Exception_Handler(next_exception_handler)
    , handler_(handler)
  {
  }

private:
  virtual VALUE handle_exception() const
  {
    try
    {
      return call_next_exception_handler();
    }
    catch(Exception_T const & ex)
    {
      handler_(ex);
      throw;
    }
  }

private:
  Functor_T handler_;
};

} // namespace detail

} // namespace Rice

#endif // Rice__detail__Exception_Handler__hpp_

