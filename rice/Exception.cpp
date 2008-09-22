#include "Exception.hpp"
#include "protect.hpp"
#include "to_from_ruby.hpp"
#include "detail/ruby.hpp"

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif

Rice::Exception::
Exception(VALUE e)
  : Exception_Base(e)
  , message_(Qnil)
  , message_guard_(&message_)
{
}

Rice::Exception::
Exception(Exception const & other)
  : Exception_Base(other)
  , message_(other.message_)
  , message_guard_(&message_)
{
}

Rice::Exception::
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

Rice::String Rice::Exception::
message() const
{
  return protect(rb_funcall, value(), rb_intern("message"), 0);
}

char const * Rice::Exception::
what() const throw()
{
  message_ = message();
  return from_ruby<char const *>(message_);
}

