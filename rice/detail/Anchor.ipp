namespace Rice
{
  namespace detail
  {
    inline Anchor::Anchor(VALUE value)
    {
      // rb_gc_register_address() can trigger GC, so we must register the
      // empty this->value_ slot before storing a heap VALUE in it.
      // RB_GC_GUARD(value) keeps the ctor argument alive through the end of
      // this method until the registered slot has been updated.
      if (!RB_SPECIAL_CONST_P(value))
      {
        Anchor::registerExitHandler();
        detail::protect(rb_gc_register_address, &this->value_);
        this->registered_ = true;
      }
      this->value_ = value;
      RB_GC_GUARD(value);
    }

    inline Anchor::~Anchor()
    {
      if (Anchor::enabled_ && this->registered_)
      {
        detail::protect(rb_gc_unregister_address, &this->value_);
      }
      // Ruby auto detects VALUEs in the stack, so make sure up in case this object is on the stack
      this->registered_ = false;
      this->value_ = Qnil;
    }

    inline VALUE Anchor::get() const
    {
      return this->value_;
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
