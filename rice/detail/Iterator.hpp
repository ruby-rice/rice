#ifndef Rice_Iterator__hpp_
#define Rice_Iterator__hpp_

#include "detail/method_data.hpp"
#include "protect.hpp"
#include "Identifier.hpp"

namespace Rice
{
  namespace detail
  {

    template<typename T, typename Iterator_Return_T>
    class Iterator
    {
    public:
      static VALUE call(VALUE self);

    public:
      Iterator(Iterator_Return_T(T::* begin)(), Iterator_Return_T(T::* end)());
      virtual ~Iterator() = default;
      VALUE operator()(VALUE self);

    private:
      Iterator_Return_T(T::* begin_)();
      Iterator_Return_T(T::* end_)();
    };

  } // detail
} // Rice

#endif // Rice_Iterator__hpp_

