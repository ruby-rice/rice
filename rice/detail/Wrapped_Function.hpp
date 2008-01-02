#ifndef Rice__detail__Wrapped_Function__hpp_
#define Rice__detail__Wrapped_Function__hpp_

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

class Wrapped_Function
{
public:
  Wrapped_Function(RUBY_METHOD_FUNC func, int arity)
    : func_(func)
    , arity_(arity)
  {
  }

  RUBY_METHOD_FUNC func() const { return func_; }
  int arity() const { return arity_; }

private:
  RUBY_METHOD_FUNC func_;
  int arity_;
};

} // detail

} // Rice

#endif // Rice__detail__Wrapped_Function__hpp_
