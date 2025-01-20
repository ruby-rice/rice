namespace Rice
{
  inline JumpException::JumpException(ruby_tag_type tag) : tag(tag)
  {
    this->createMessage();
  }

  inline const char* JumpException::what() const noexcept
  {
    return this->message_.c_str();
  }

  inline void JumpException::createMessage()
  {
    switch (this->tag)
    {
      case RUBY_TAG_NONE:
        this->message_ = "No error";
        break;
      case RUBY_TAG_RETURN:
        this->message_ = "Unexpected return";
        break;
      case RUBY_TAG_NEXT:
        this->message_ = "Unexpected next";
        break;
      case RUBY_TAG_BREAK:
        this->message_ = "Unexpected break";
        break;
      case RUBY_TAG_REDO:
        this->message_ = "Unexpected redo";
        break;
      case RUBY_TAG_RETRY:
        this->message_ = "Retry outside of rescue clause";
        break;
      case RUBY_TAG_THROW:
        this->message_ = "Unexpected throw";
      case RUBY_TAG_RAISE:
        this->message_ = "Ruby exception was thrown";
        break;
      case RUBY_TAG_FATAL:
        this->message_ = "Fatal error";
        break;
      case RUBY_TAG_MASK:
        this->message_ = "Mask error";
        break;
    }
  }
} // namespace Rice
