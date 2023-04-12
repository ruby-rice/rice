#ifndef Rice_Iterator__hpp_
#define Rice_Iterator__hpp_

#include "../Identifier.hpp"

namespace Rice::detail
{
  template<typename T, typename Iterator_T>
  class Iterator
  {
  public:
    static VALUE call(VALUE self);

  public:
    Iterator(Identifier name, Iterator_T(T::* begin)(), Iterator_T(T::* end)());
    virtual ~Iterator() = default;
    VALUE operator()(VALUE self);

  private:
    Identifier name_;
    Iterator_T(T::* begin_)();
    Iterator_T(T::* end_)();
  };
}

#endif // Rice_Iterator__hpp_