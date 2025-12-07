
RICE_BEGIN_NAMESPACE
  inline Exception::Exception(VALUE exception) : exception_(exception)
  {
  }

  template <typename... Parameter_Ts>
  inline Exception::Exception(const Exception& other, char const* fmt, Parameter_Ts&&...args)
    : Exception(other.class_of(), fmt, std::forward<Parameter_Ts>(args)...)
  {
  }

  template <typename... Parameter_Ts>
  inline Exception::Exception(const VALUE exceptionClass, char const* fmt, Parameter_Ts&&...args)
  {
    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-security"
    #endif

    size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Parameter_Ts>(args)...);
    this->message_ = std::string(size, '\0');

    // size+1 avoids truncating the string. Otherwise snprintf writes n - 1 characters
    // to allow space for null character but we don't need that since std::string
    // will add a null character internally at n + 1
    std::snprintf(&this->message_[0], size + 1, fmt, std::forward<Parameter_Ts>(args)...);

    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif

    // Now create the Ruby exception
    this->exception_ = detail::protect(rb_exc_new2, exceptionClass, this->message_.c_str());
  }

  inline char const* Exception::what() const noexcept
  {
    if (this->message_.empty())
    {
      // This isn't protected because if it fails then either we could eat the exception
      // (not good) or crash the program (better)
      VALUE rubyMessage = rb_funcall(this->exception_, rb_intern("message"), 0);
      this->message_ = std::string(RSTRING_PTR(rubyMessage), RSTRING_LEN(rubyMessage));
    }
    return this->message_.c_str();
  }

  inline VALUE Exception::class_of() const
  {
    return detail::protect(rb_class_of, this->exception_);
  }

  inline VALUE Exception::value() const
  {
    return this->exception_;
  }
RICE_END_NAMESPACE
