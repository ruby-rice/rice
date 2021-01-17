#ifndef Rice__detail__Exception_Handler_defn__hpp_
#define Rice__detail__Exception_Handler_defn__hpp_

#include <memory>
#include "ruby.hpp"

namespace Rice
{

namespace detail
{

/* An abstract class for converting C++ exceptions to ruby exceptions.  It's used
   like this:

   try
   {
   }
   catch(...)
   {
     handler->handle_exception();
   }

 If an exception is thrown the handler will pass the exception up the
 chain, then the last handler in the chain will throw the exception
 down the chain until a lower handler can handle it, e.g.:

 try
 {
   return call_next_exception_handler();
 }
 catch(MyException const & ex)
 {
   throw Rice::Exception(rb_cMyException, "%s", ex.what());
  } 
  
  Memory management. Handlers are created by the ModuleBase constructor. When the
  module defines a new Ruby method, metadata  is stored on the Ruby klass including 
  the exception handler. Since the metadata outlives the module, handlers are stored
  using std::shared_ptr. Thus the Module (or its inherited children) can be destroyed
  without corrupting the metadata references to the shared exception handler. */

class Exception_Handler
{
public:
  Exception_Handler() = default;
  virtual ~Exception_Handler() = default;

  // Don't allow copying or assignment
  Exception_Handler(const Exception_Handler& other) = delete;
  Exception_Handler& operator=(const Exception_Handler& other) = delete;

  virtual VALUE handle_exception() const = 0;
};

// The default exception handler just rethrows the exception.  If there
// are other handlers in the chain, they will try to handle the rethrown
// exception.
class Default_Exception_Handler
  : public Exception_Handler
{
public:
  virtual VALUE handle_exception() const override;
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
  Functor_Exception_Handler(Functor_T handler,
                            std::shared_ptr<Exception_Handler> next_exception_handler);

  virtual VALUE handle_exception() const override;

private:
  Functor_T handler_;
  std::shared_ptr<Exception_Handler> next_exception_handler_;
};

} // namespace detail

} // namespace Rice

#endif // Rice__detail__Exception_Handler_defn__hpp_

