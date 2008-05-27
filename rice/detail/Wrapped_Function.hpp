#ifndef Rice__detail__Wrapped_Function__hpp_
#define Rice__detail__Wrapped_Function__hpp_

#include "ruby.hpp"
#include "../Data_Object.hpp"

namespace Rice
{

namespace detail
{
  class Exception_Handler;

class Wrapped_Function
{
public:
  Wrapped_Function(RUBY_METHOD_FUNC func, int arity, Object handler)
    : func_(func)
    , arity_(arity)
    , handler_(handler)
  {
  }

  RUBY_METHOD_FUNC func() const { return func_; }
  int arity() const { return arity_; }
  Exception_Handler * handler() { return Data_Object<Exception_Handler>(handler_).get(); }

  static void mark(Wrapped_Function * self)
  {
    rb_gc_mark(self->handler_);
  }

private:
  RUBY_METHOD_FUNC func_;
  int arity_;
  Object handler_;
};

} // detail

} // Rice

#endif // Rice__detail__Wrapped_Function__hpp_
