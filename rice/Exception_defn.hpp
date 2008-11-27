#ifndef Rice__Exception_defn__hpp_
#define Rice__Exception_defn__hpp_

#include "Exception_Base_defn.hpp"
#include "String.hpp"
#include "Address_Registration_Guard_defn.hpp"

#include <stdexcept>
#include "detail/ruby.hpp"

namespace Rice
{

//! A placeholder for Ruby exceptions.
/*! You can use this to safely throw a Ruby exception using C++ syntax:
 *  \code
 *    VALUE foo(VALUE self) {
 *      RUBY_TRY {
 *        throw Rice::Exception(rb_eMyException, "uh oh!");
 *      RUBY_CATCH
 *    }
 *  \endcode
 */
class Exception
  : public Exception_Base
{
public:
  //! Construct a Exception with the exception e.
  explicit Exception(VALUE e);

  //! Copy constructor.
  Exception(Exception const & other);

  //! Construct a Exception with printf-style formatting.
  /*! \param exc either an exception object or a class that inherits
   *  from Exception.
   *  \param fmt a printf-style format string
   *  \param ... the arguments to the format string.
   */
  Exception(Object exc, char const * fmt, ...);

  //! Destructor
  virtual ~Exception() throw() { }

  //! Get the message the exception holds
  /*! \return the result of calling message() on the underlying
   *  exception object.
   */
  String message() const;

  //! Get message as a char const *.
  /*! If message is a non-string object, then this function will attempt
   *  to throw an exception (which it can't do because of the no-throw
   *  specification).
   *  \return the underlying C pointer of the underlying message object.
   */
  virtual char const * what() const throw();

private:
  // Keep message around in case someone calls what() and then the GC
  // gets invoked.
  mutable VALUE message_;
  Address_Registration_Guard message_guard_;
};

} // namespace Rice

#endif // Rice__Exception_defn__hpp_

