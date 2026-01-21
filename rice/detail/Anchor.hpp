#ifndef Rice__detail__Anchor__hpp_
#define Rice__detail__Anchor__hpp_

#include <ruby.h>

namespace Rice
{
  namespace detail
  {
    //! Internal GC anchor for a Ruby VALUE.
    /*!
     *  Anchor is a low-level adapter around the Ruby GC API.
     *  It owns a stable VALUE slot whose address is registered
     *  with the Ruby garbage collector.
     *
     *  This class encapsulates all GC registration logic and is
     *  not part of the public Rice API.
     */
    class Anchor
    {
    public:
      //! Construct an anchor for the given Ruby VALUE.
      /*!
       *  The address of the internal VALUE is registered with the
       *  Ruby GC, preventing collection while this Anchor exists.
       */
      explicit Anchor(VALUE value);

      //! Unregister the VALUE from the Ruby GC.
      ~Anchor();

      Anchor(const Anchor&) = delete;
      Anchor& operator=(const Anchor&) = delete;

      //! Retrieve the currently anchored VALUE.
      VALUE get() const;

      //! Replace the anchored VALUE.
      /*!
       *  The GC root (address) remains unchanged; only the VALUE
       *  stored at that address is updated.
       */
      void set(VALUE value);

    private:
      static void disable(VALUE);
      static void registerExitHandler();

      inline static bool enabled_ = true;
      inline static bool exitHandlerRegistered_ = false;

    private:
      //! GC-visible Ruby VALUE slot.
      VALUE value_;
    };
  }
}
#endif // Rice__detail__Anchor__hpp_
