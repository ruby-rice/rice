namespace Rice
{
  inline Pin::Pin(VALUE value)
    : anchor_(std::make_shared<detail::Anchor>(value))
  {
  }

  inline VALUE Pin::value() const
  {
    return this->anchor_->get();
  }
}
