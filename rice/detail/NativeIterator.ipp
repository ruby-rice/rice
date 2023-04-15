#include <iterator>
#include <functional>
#include <type_traits>

#include "cpp_protect.hpp"
#include "method_data.hpp"

namespace Rice::detail
{
  template <typename T, typename Iterator_Func_T>
  inline NativeIterator<T, Iterator_Func_T>::NativeIterator(Identifier name, Iterator_Func_T begin, Iterator_Func_T end) :
       name_(name), begin_(begin), end_(end)
  {
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::call(VALUE self)
  {
    using Iter_T = NativeIterator<T, Iterator_Func_T>;
    Iter_T* iterator = detail::MethodData::data<Iter_T*>();

    return cpp_protect([&]
    {
      return iterator->operator()(self);
    });
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::createRubyEnumerator(VALUE self)
  {
    auto rb_size_function = [](VALUE recv, VALUE argv, VALUE eobj) -> VALUE
    {
      // Since we can't capture VALUE self from above (because then we can't send
      // this lambda to rb_enumeratorize_with_size), extract it from recv
      return cpp_protect([&]
      {
        // Get the iterator instance
        using Iter_T = NativeIterator<T, Iterator_Func_T>;
        // Class is easy
        VALUE klass = protect(rb_class_of, recv);
        // Read the method_id from an attribute we added to the enumerator instance
        ID method_id = protect(rb_ivar_get, eobj, rb_intern("rice_method"));
        Iter_T* iterator = detail::MethodData::data<Iter_T*>(klass, method_id);

        // Get the wrapped C++ instance
        T* receiver = detail::From_Ruby<T*>().convert(recv);

        // Get the distance
        Iterator_T begin = std::invoke(iterator->begin_, *receiver);
        Iterator_T end = std::invoke(iterator->end_, *receiver);
        Difference_T distance = std::distance(begin, end);

        return detail::To_Ruby<Difference_T>().convert(distance);
      });
    };

    VALUE enumerator = protect(rb_enumeratorize_with_size, self, this->name_.to_sym(), 0, nullptr, rb_size_function);
    
    // Hack the enumerator object by storing name_ on the enumerator object so
    // the rb_size_function above has access to it
    protect(rb_ivar_set, enumerator, rb_intern("rice_method"), this->name_);

    return enumerator;
  }

  template<typename T, typename Iterator_Func_T>
  inline VALUE NativeIterator<T, Iterator_Func_T>::operator()(VALUE self)
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
        protect(rb_yield, detail::To_Ruby<Reference_T>().convert(*it));
      }

      return self;
    }
  }
}