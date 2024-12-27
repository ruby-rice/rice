namespace Rice::detail
{
  inline void Rice::detail::DefaultHandler::operator()() const
  {
    // This handler does nothing, it just rethrows the exception so it can be handled
    throw;
  }
}