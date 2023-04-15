#ifndef Rice_NativeIterator__hpp_
#define Rice_NativeIterator__hpp_

#include "../Identifier.hpp"
#include "function_traits.hpp"

namespace Rice::detail
{
  template<typename T, typename Iterator_Func_T>
  class NativeIterator
  {
  public:
    using Iterator_T = typename function_traits<Iterator_Func_T>::return_type;
    using Reference_T = typename std::iterator_traits<Iterator_T>::reference;
    using Difference_T = typename std::iterator_traits<Iterator_T>::difference_type;

  public:
    static VALUE call(VALUE self);

  public:
    NativeIterator(Identifier name, Iterator_Func_T begin, Iterator_Func_T end);
    virtual ~NativeIterator() = default;
    VALUE operator()(VALUE self);

  private:
    VALUE createRubyEnumerator(VALUE self);

  private:
    Identifier name_;
    Iterator_Func_T begin_;
    Iterator_Func_T end_;
  };
}
#include "NativeIterator.ipp"

#endif // Rice_NativeIterator__hpp_