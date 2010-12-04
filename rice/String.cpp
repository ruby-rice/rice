#include "String.hpp"
#include "Builtin_Object.hpp"
#include "protect.hpp"
#include "detail/ruby.hpp"

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif

Rice::String::
String()
  : Builtin_Object<RString, T_STRING>(protect(rb_str_new2, ""))
{
}

Rice::String::
String(VALUE v)
  : Builtin_Object<RString, T_STRING>(v)
{
}

Rice::String::
String(Object v)
  : Builtin_Object<RString, T_STRING>(v)
{
}

Rice::String::
String(char const * s)
  : Builtin_Object<RString, T_STRING>(protect(rb_str_new2, s))
{
}

Rice::String::
String(std::string const & s)
  : Builtin_Object<RString, T_STRING>(protect(rb_str_new, s.data(), s.length()))
{
}

Rice::String::
String(Identifier id)
  : Builtin_Object<RString, T_STRING>(protect(rb_str_new2, id.c_str()))
{
}

Rice::String Rice::String::
format(char const * fmt, ...)
{
  va_list args;
  char buf[BUFSIZ];
  
  va_init_list(args, fmt);
  vsnprintf(buf, BUFSIZ, fmt, args);
  buf[BUFSIZ - 1] = '\0';
  va_end(args);

  String s = String(buf);
  return s;
}

size_t Rice::String::
length() const
{
  return RSTRING_LEN(value());
}

char Rice::String::
operator[](ptrdiff_t index) const
{
  return RSTRING_PTR(value())[index];
}

char const * Rice::String::
c_str() const
{
  return RSTRING_PTR(value());
}

std::string Rice::String::
str() const
{
  return std::string(RSTRING_PTR(value()), length());
}

Rice::Identifier Rice::String::
intern() const
{
  return rb_intern(c_str());
}

