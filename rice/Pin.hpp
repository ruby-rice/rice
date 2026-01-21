#ifndef Rice__Pin__hpp_
#define Rice__Pin__hpp_

namespace Rice
{
  //! Strong lifetime policy for a Ruby VALUE.
  /*!
   *  Pin represents a Ruby VALUE whose lifetime is explicitly
   *  extended by C++ code.
   *
   *  Internally, Pin uses a GC Anchor to keep the VALUE alive.
   *  Copying a Pin shares the underlying anchor; moving is cheap.
   *
   *  This type is intended for C++-owned objects that store Ruby
   *  values but are not themselves owned by Ruby and thus do not
   *  participate in the GC via a mark function.
   */
  class Pin
  {
  public:
    //! Construct a strong pin for a Ruby VALUE.
    explicit Pin(VALUE value);

    // Shared-anchor semantics.
    Pin(const Pin&) = default;
    Pin& operator=(const Pin&) = default;

    Pin(Pin&&) noexcept = default;
    Pin& operator=(Pin&&) noexcept = default;

    //! Replace the pinned Ruby VALUE.
    void set(VALUE value);

    //! Retrieve the pinned Ruby VALUE.
    VALUE get() const;

  private:
    //! Shared ownership of the internal GC anchor.
    std::shared_ptr<detail::Anchor> anchor_;
  };
}
#endif // Rice__Pin__hpp_
