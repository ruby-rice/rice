#ifndef Rice_Iterator__ipp_
#define Rice_Iterator__ipp_

#include <iterator>
#include <functional>

#include "../Data_Object_defn.hpp"
#include "method_data.hpp"

namespace Rice
{
  namespace detail
  {

    template <typename T, typename Iterator_T>
    inline Iterator<T, Iterator_T>::
      Iterator(Iterator_T(T::* begin)(), Iterator_T(T::* end)()) :
      begin_(begin), end_(end)
    {
    }

    template<typename T, typename Iterator_T>
    inline VALUE Iterator<T, Iterator_T>::
    call(VALUE self)
    {
      using Iter_T = Iterator<T, Iterator_T>;
      Iter_T* iterator = detail::MethodData::data<Iter_T*>();
      return iterator->operator()(self);
    }

    template<typename T, typename Iterator_T>
    inline VALUE Iterator<T, Iterator_T>::
    operator()(VALUE self)
    {
      using Value_T = typename std::iterator_traits<Iterator_T>::value_type;

      Data_Object<T> obj(self);
      Iterator_T it = std::invoke(this->begin_, *obj);
      Iterator_T end = std::invoke(this->end_, *obj);

      for (; it != end; ++it)
      {
        protect(rb_yield, detail::To_Ruby<Value_T>::convert(*it));
      }

      return self;
    }
  } // namespace detail
} // namespace Rice

#endif // Rice_Iterator__ipp_

