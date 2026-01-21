namespace Rice
{
  inline Pin::Pin(VALUE value)
    : anchor_(std::make_shared<detail::Anchor>(value))
  {
  }

  inline void Pin::set(VALUE value)
  {
    this->anchor_->set(value);
  }

  inline VALUE Pin::get() const
  {
    return this->anchor_->get();
  }
}
