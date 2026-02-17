namespace Rice
{
  inline Pin::Pin(VALUE value)
    : anchor_(std::make_shared<detail::Anchor>(value))
  {
  }

  inline VALUE Pin::value() const
  {
    // Anchor can be nil after a move
    if (this->anchor_)
    {
      return this->anchor_->get();
    }
    else
    {
      return Qnil;
    }
  }
}
