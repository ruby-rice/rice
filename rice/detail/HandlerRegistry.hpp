#ifndef Rice__detail__HandlerRegistry__hpp_
#define Rice__detail__HandlerRegistry__hpp_

#include <functional>

namespace Rice::detail
{
  class HandlerRegistry
  {
  public:
    HandlerRegistry();
    void set(std::function<void()> handler);
    std::function<void()> handler() const;

  private:
    std::function<void()> handler_;
  };
} // namespace Rice::detail

#endif // Rice__detail__HandlerRegistry__hpp_

