namespace Rice::detail
{
  inline HandlerRegistry::HandlerRegistry() : handler_(DefaultHandler())
  {
  }

  inline void HandlerRegistry::set(std::function<void()> handler)
  {
    this->handler_ = handler;
  }

  inline std::function<void()> HandlerRegistry::handler() const
  {
    return this->handler_;
  }
}