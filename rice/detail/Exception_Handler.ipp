#include "../Data_Type_defn.hpp"

inline
Rice::detail::Exception_Handler::
Exception_Handler(
    Data_Object<Exception_Handler> next_exception_handler)
  : next_exception_handler_(next_exception_handler)
  , next_exception_handler_guard_(&next_exception_handler_)
{
}

inline
Rice::detail::Exception_Handler::
~Exception_Handler()
{
}

inline
VALUE
Rice::detail::Exception_Handler::
call_next_exception_handler() const
{
  return next_exception_handler_->handle_exception();
}

inline Rice::detail::Default_Exception_Handler::
Default_Exception_Handler()
  : Exception_Handler(
      Data_Object<Exception_Handler>(0, rb_cObject))
{
}

inline
VALUE
Rice::detail::Default_Exception_Handler::
handle_exception() const
{
  throw;
}

template <typename Exception_T, typename Functor_T>
inline
Rice::detail::Functor_Exception_Handler<Exception_T, Functor_T>::
Functor_Exception_Handler(
    Functor_T handler,
    Data_Object<Exception_Handler> next_exception_handler)
  : Exception_Handler(next_exception_handler)
  , handler_(handler)
{
}

template <typename Exception_T, typename Functor_T>
inline
VALUE
Rice::detail::Functor_Exception_Handler<Exception_T, Functor_T>::
handle_exception() const
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

