RICE_DETAIL_BEGIN_NAMESPACE
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
RICE_DETAIL_END_NAMESPACE
