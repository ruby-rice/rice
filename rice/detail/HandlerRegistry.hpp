#ifndef Rice__detail__HandlerRegistry__hpp_
#define Rice__detail__HandlerRegistry__hpp_

#include <functional>

RICE_DETAIL_BEGIN_NAMESPACE
  class HandlerRegistry
  {
  public:
    HandlerRegistry();
    void set(std::function<void()> handler);
    std::function<void()> handler() const;

  private:
    std::function<void()> handler_;
  };
RICE_DETAIL_END_NAMESPACE

#endif // Rice__detail__HandlerRegistry__hpp_

