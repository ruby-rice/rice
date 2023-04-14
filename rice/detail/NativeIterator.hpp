#ifndef Rice_NativeIterator__hpp_
#define Rice_NativeIterator__hpp_

#include "../Identifier.hpp"

namespace Rice::detail
{
  template<typename T, typename Iterator_T>
  class NativeIterator
  {
  public:
    static VALUE call(VALUE self);

  public:
    NativeIterator(Identifier name, Iterator_T(T::* begin)(), Iterator_T(T::* end)());
    virtual ~NativeIterator() = default;
    VALUE operator()(VALUE self);

  private:
    VALUE createRubyEnumerator(VALUE self);

  private:
    Identifier name_;
    Iterator_T(T::* begin_)();
    Iterator_T(T::* end_)();
  };
}
#include "NativeIterator.ipp"

#endif // Rice_NativeIterator__hpp_