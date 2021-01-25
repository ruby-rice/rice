#ifndef Rice_Iterator__ipp_
#define Rice_Iterator__ipp_

namespace Rice
{
  namespace detail
  {

    template <typename T, typename Iterator_Return_T>
    inline Iterator<T, Iterator_Return_T>::
      Iterator(Iterator_Return_T(T::* begin)(), Iterator_Return_T(T::* end)()) :
      begin_(begin), end_(end)
    {
    }

    template<typename T, typename Iterator_Return_T>
    inline VALUE Iterator<T, Iterator_Return_T>::
      call(VALUE self)
    {
      using Iterator_T = Iterator<T, Iterator_Return_T>;
      Iterator_T* iterator = std::any_cast<Iterator_T*>(detail::MethodData::data());
      return iterator->operator()(self);
    }

    template<typename T, typename Iterator_Return_T>
    inline VALUE Iterator<T, Iterator_Return_T>::
      operator()(VALUE self)
    {
      Data_Object<T> obj(self);
      Iterator_Return_T it = (*obj.*begin_)();
      Iterator_Return_T end = (*obj.*end_)();

      for (; it != end; ++it)
      {
        Rice::protect(rb_yield, detail::To_Ruby<Iterator_Return_T>::convert(it));
      }

      return self;
    }

    template<typename T, typename Iterator_Return_T>
    void define_iterator(Identifier name, Iterator_Return_T(T::* begin)(), Iterator_Return_T(T::* end)())
    {
      using Iterator_T = Iterator<T, Iterator_Return_T>;
      Iterator_T* iterator = new Iterator_T(begin, end);
      Data_Object<Iterator_T> wrapper(iterator);

      define_method_with_data(
        Data_Type<T>::klass(),
        name,
        (RUBY_METHOD_FUNC)wrapper->call,
        0,
        wrapper);
    }
  } // namespace detail
} // namespace Rice

#endif // Rice_Iterator__ipp_

