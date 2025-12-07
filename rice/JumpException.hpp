#ifndef Rice__JumpException__hpp_
#define Rice__JumpException__hpp_

RICE_BEGIN_NAMESPACE
  //! A placeholder for Ruby longjmp data.
  /*! When a Ruby exception is caught, the tag used for the longjmp is stored in
   *  a Jump_Tag, then later passed to rb_jump_tag() when there is no more
   *  C++ code to pass over.
   */
  class JumpException : public std::exception
  {
  public:
    // Copied from vm_core.h
    enum ruby_tag_type {
      RUBY_TAG_NONE = 0x0,
      RUBY_TAG_RETURN = 0x1,
      RUBY_TAG_BREAK = 0x2,
      RUBY_TAG_NEXT = 0x3,
      RUBY_TAG_RETRY = 0x4,
      RUBY_TAG_REDO = 0x5,
      RUBY_TAG_RAISE = 0x6,
      RUBY_TAG_THROW = 0x7,
      RUBY_TAG_FATAL = 0x8,
      RUBY_TAG_MASK = 0xf
    };

  public:
    JumpException(ruby_tag_type tag);
    virtual const char* what() const noexcept override;

  public:
    //! The tag being held.
    ruby_tag_type tag;

  private:
    void createMessage();

  private:
    std::string message_;
  };
RICE_END_NAMESPACE

#endif // Rice__JumpException__hpp_
