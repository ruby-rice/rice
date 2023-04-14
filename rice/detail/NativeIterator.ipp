#include <iterator>
#include <functional>
#include <type_traits>

#include "cpp_protect.hpp"
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

    return cpp_protect([&]
    {
      return iterator->operator()(self);
    });
  }

  template<typename T, typename Iterator_T>
  inline VALUE NativeIterator<T, Iterator_T>::createRubyEnumerator(VALUE self)
  {
    auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
    {
      // Since we can't capture VALUE self from above (because then we can't send
      // this lambda to rb_enumeratorize_with_size), extract it from recv
      return cpp_protect([&]
      {
        // Get the iterator instance
        using Iter_T = NativeIterator<T, Iterator_T>;
        Iter_T* iterator = detail::MethodData::data<Iter_T*>();

        // Get the wrapped C++ instance
        T* receiver = detail::From_Ruby<T*>().convert(recv);

        // Get the distance
        Iterator_T begin = std::invoke(iterator->begin_, *receiver);
        Iterator_T end = std::invoke(iterator->end_, *receiver);
        Difference_T distance = std::distance(begin, end);

        return detail::To_Ruby<Difference_T>().convert(distance);
      });
    };

    return protect(rb_enumeratorize_with_size, self, this->name_.to_sym(), 0, nullptr, rb_size_function);
  }

  template<typename T, typename Iterator_T>
  inline VALUE NativeIterator<T, Iterator_T>::operator()(VALUE self)
  {
    if (!protect(rb_block_given_p))
    {
      return createRubyEnumerator(self);
    }
    else
    {
      T* receiver = detail::From_Ruby<T*>().convert(self);
      Iterator_T it = std::invoke(this->begin_, *receiver);
      Iterator_T end = std::invoke(this->end_, *receiver);

      for (; it != end; ++it)
      {
        protect(rb_yield, detail::To_Ruby<Value_T>().convert(*it));
      }

      return self;
    }
  }
}