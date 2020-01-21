#ifndef Rice__Exception_defn__hpp_
#define Rice__Exception_defn__hpp_

#include "Exception_Base_defn.hpp"
#include "String.hpp"
#include "Address_Registration_Guard_defn.hpp"

#include <stdexcept>
#include "detail/ruby.hpp"

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif

// TODO Why do I need this?
template<> inline std::string from_ruby<std::string>(Rice::Object x);

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
  explicit Exception(VALUE e)
    : Exception_Base(e)
    , message_(Qnil)
    , message_guard_(&message_)
  {
  }

  //! Copy constructor.
  Exception(Exception const & other)
    : Exception_Base(other)
    , message_(other.message_)
    , message_guard_(&message_)
  {
  }

  //! Construct a Exception with printf-style formatting.
  /*! \param exc either an exception object or a class that inherits
   *  from Exception.
   *  \param fmt a printf-style format string
   *  \param ... the arguments to the format string.
   */
  Exception(Object exc, char const * fmt, ...)
    : Exception_Base(Qnil)
    , message_(Qnil)
    , message_guard_(&message_)
  {
    va_list args;
    char buf[BUFSIZ];

    va_init_list(args, fmt);
    vsnprintf(buf, BUFSIZ, fmt, args);
    buf[BUFSIZ - 1] = '\0';
    va_end(args);

    set_value(protect(rb_exc_new2, exc, buf));
  }

  //! Destructor
  virtual ~Exception() throw() { }

  //! Get the message the exception holds
  /*! \return the result of calling message() on the underlying
   *  exception object.
   */
  String message() const
  {
    return protect(rb_funcall, value(), rb_intern("message"), 0);
  }

  //! Get message as a char const *.
  /*! If message is a non-string object, then this function will attempt
   *  to throw an exception (which it can't do because of the no-throw
   *  specification).
   *  \return the underlying C pointer of the underlying message object.
   */
  virtual char const * what() const throw()
  {
    message_ = message();
    return from_ruby<std::string>(message_).c_str();
  }

private:
  // Keep message around in case someone calls what() and then the GC
  // gets invoked.
  mutable VALUE message_;
  Address_Registration_Guard message_guard_;
};

} // namespace Rice

#endif // Rice__Exception_defn__hpp_

