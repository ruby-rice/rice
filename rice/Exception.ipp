#ifndef Rice__Exception__ipp_
#define Rice__Exception__ipp_

#include "detail/from_ruby.hpp"

inline Rice::Exception::
Exception(VALUE exception): exception_(exception)
{
}

template <typename... Arg_Ts>
inline Rice::Exception::
Exception(const Exception& other, char const* fmt, Arg_Ts&&...args)
: Exception(other.class_of(), fmt, std::forward<Arg_Ts>(args)...)
{
}

template <typename... Arg_Ts>
inline Rice::Exception::
Exception(const VALUE exceptionClass, char const* fmt, Arg_Ts&&...args)
{
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

  size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
  this->message_ = std::string(size, '\0');

  // size+1 avoids trunctaing the string. Otherwise snprintf writes n - 1 characters
  // to allow space for null character but we don't need that since std::string
  // will add a null character internally at n + 1
  std::snprintf(&this->message_[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

  // Now create the Ruby exception
  this->exception_ = detail::protect(rb_exc_new2, exceptionClass, this->message_.c_str());
}

inline char const* Rice::Exception::
what() const noexcept
{
  if (this->message_.empty())
  {
    // Cache the message - this allows the returned pointer to be valid for the
    // lifetime of this exception instance.
    VALUE rubyMessage = rb_funcall(this->exception_, rb_intern("message"), 0);
    //this->message_ = detail::From_Ruby<std::string>::convert(rubyMessage);
    this->message_ = std::string(RSTRING_PTR(rubyMessage), RSTRING_LEN(rubyMessage));
  }
  return this->message_.c_str();
}

inline VALUE Rice::Exception::
class_of() const
{
  return detail::protect(rb_class_of, this->exception_);
}

inline VALUE Rice::Exception::
value() const
{
  return this->exception_;
}
#endif // Rice__Exception__ipp_