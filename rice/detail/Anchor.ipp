namespace Rice
{
  namespace detail
  {
    inline Anchor::Anchor(VALUE value) : value_(value)
    {
      if (value != Qnil)
      {
        Anchor::registerExitHandler();
        detail::protect(rb_gc_register_address, &this->value_);
      }
    }

    inline Anchor::~Anchor()
    {
      if (Anchor::enabled_ && this->value_ != Qnil)
      {
        detail::protect(rb_gc_unregister_address, &this->value_);
      }
      // Ruby auto detects VALUEs in the stack, so make sure up in case this object is on the stack
      this->value_ = Qnil;
    }

    inline VALUE Anchor::get() const
    {
      return this->value_;
    }

    inline void Anchor::set(VALUE value)
    {
      this->value_ = value;
    }

    // This will be called by ruby at exit - we want to disable further unregistering
    inline void Anchor::disable(VALUE)
    {
      Anchor::enabled_ = false;
    }

    inline void Anchor::registerExitHandler()
    {
      if (!Anchor::exitHandlerRegistered_)
      {
        detail::protect(rb_set_end_proc, &Anchor::disable, Qnil);
        Anchor::exitHandlerRegistered_ = true;
      }
    }
  }
}