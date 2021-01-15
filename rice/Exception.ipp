#ifndef Rice__Exception__ipp_
#define Rice__Exception__ipp_

#include "protect.hpp"
#include "detail/from_ruby.hpp"

inline Rice::Exception::
Exception(VALUE exception)
{
  VALUE rubyMessage = protect(rb_funcall, exception, rb_intern("message"), 0);
  this->message_ = detail::From_Ruby<std::string>::convert(rubyMessage);
  this->exceptionClass_ = rb_class_of(exception);
  this->exception_ = exception;
}

template <typename... Arg_Ts>
inline Rice::Exception::
Exception(const Exception& other, char const* fmt, Arg_Ts&&...args)
: Exception(other.exceptionClass_, fmt, std::forward<Arg_Ts>(args)...)
{
}

template <typename... Arg_Ts>
inline Rice::Exception::
Exception(const VALUE exceptionClass, char const* fmt, Arg_Ts&&...args)
{
  this->exceptionClass_ = exceptionClass;
  size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
  this->message_ = std::string(size, '\0');

  // size+1 avoids trunctaing the string. Otherwise snprintf writes n - 1 characters
  // to allow space for null character but we don't need that since std::string
  // will add a null character internally at n + 1
  std::snprintf(&this->message_[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);
}

inline const std::string Rice::Exception::
message() const
{
  return this->message_;
}

inline char const* Rice::Exception::
what() const noexcept
{
  return this->message_.c_str();
}

inline VALUE Rice::Exception::
class_of() const
{
  return this->exceptionClass_;
}

inline VALUE Rice::Exception::
value() const
{
  if (this->exception_ == Qnil)
  {
    this->exception_ = protect(rb_exc_new2, this->exceptionClass_, this->what());
  }
  return this->exception_;
}
#endif // Rice__Exception__ipp_