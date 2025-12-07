RICE_DETAIL_BEGIN_NAMESPACE
  inline void DefaultHandler::operator()() const
  {
    // This handler does nothing, it just rethrows the exception so it can be handled
    throw;
  }
RICE_DETAIL_END_NAMESPACE
