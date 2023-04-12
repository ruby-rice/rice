#ifndef Rice_NativeIterator__ipp_
#define Rice_NativeIterator__ipp_

#include <iterator>
#include <functional>

#include "../Data_Object_defn.hpp"
#include "method_data.hpp"

namespace Rice::detail
{
  template <typename T, typename Iterator_T>
  inline NativeIterator<T, Iterator_T>::NativeIterator(Identifier name, Iterator_T(T::* begin)(), Iterator_T(T::* end)()) :
       name_(name), begin_(begin), end_(end)
  {
  }

  template<typename T, typename Iterator_T>
  inline VALUE NativeIterator<T, Iterator_T>::call(VALUE self)
  {
    using Iter_T = NativeIterator<T, Iterator_T>;
    Iter_T* iterator = detail::MethodData::data<Iter_T*>();
    return iterator->operator()(self);
  }

  template<typename T, typename Iterator_T>
  inline VALUE NativeIterator<T, Iterator_T>::operator()(VALUE self)
  {
    if (!rb_block_given_p())
    {
      return rb_enumeratorize(self, this->name_.to_sym(), 0, nullptr);
    }
    else
    {
      using Value_T = typename std::iterator_traits<Iterator_T>::value_type;

      Data_Object<T> obj(self);
      Iterator_T it = std::invoke(this->begin_, *obj);
      Iterator_T end = std::invoke(this->end_, *obj);

      for (; it != end; ++it)
      {
        protect(rb_yield, detail::To_Ruby<Value_T>().convert(*it));
      }

      return self;
    }
  }
}
#endif // Rice_NativeIterator__ipp_