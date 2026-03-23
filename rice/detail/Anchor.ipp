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
        rb_gc_unregister_address(&this->value_);
      }
      this->registered_ = false;
      this->value_ = Qnil;
    }

    inline VALUE Anchor::get() const
    {
      return this->value_;
    }

    inline void Anchor::registerExitHandler()
    {
      if (!Anchor::exitHandlerRegistered_)
      {
        // Use ruby_vm_at_exit which fires AFTER the VM is destroyed,
        // not rb_set_end_proc which fires BEFORE. rb_set_end_proc
        // runs as an end_proc in LIFO order alongside at_exit blocks,
        // so its timing depends on require order — if the extension
        // loads after minitest/autorun, the disable callback runs
        // before tests execute, causing Anchor destructors to skip
        // rb_gc_unregister_address and leave dangling root pointers.
        ruby_vm_at_exit([](ruby_vm_t*) { Anchor::enabled_ = false; });
        Anchor::exitHandlerRegistered_ = true;
      }
    }
  }
}
