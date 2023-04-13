#ifndef Rice_NativeIterator__ipp_
#define Rice_NativeIterator__ipp_

#include <iterator>
#include <functional>
#include <type_traits>

#include "cpp_protect.hpp"
#include "method_data.hpp"
#include "../Data_Object_defn.hpp"

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

  template<typename T, typename = void>
  struct has_size : std::false_type {};

  template<typename T>
  struct has_size<T, std::void_t<std::is_member_function_pointer<decltype(&T::size)>>> : std::true_type {};

  template<typename T, typename Iterator_T>
  inline VALUE NativeIterator<T, Iterator_T>::createRubyEnumerator(VALUE self)
  {
    using SizeFunc_T = VALUE(*)(VALUE recv, VALUE argv, VALUE eobj);
    SizeFunc_T rb_size_function = nullptr;

    if constexpr (has_size<T>::value)
    {
      rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
      {
        // Since we can't capture VALUE self from above (because then we can't send
        // this lambda to rb_enumeratorize_with_size), extract it from recv
        return cpp_protect([&]
        {
          T* receiver = Data_Object<T>::from_ruby(recv);
          return detail::To_Ruby<size_t>().convert(receiver->size());
        });
      };
    }

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