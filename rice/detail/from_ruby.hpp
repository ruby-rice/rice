#ifndef Rice__detail__from_ruby__hpp_
#define Rice__detail__from_ruby__hpp_

namespace Rice
{
  namespace detail
  {
    template<typename T>
    struct from_ruby_
    {
      typedef T Retval_T;

      static T convert(VALUE x);
    };

    template<typename T>
    struct from_ruby_<T *>
    {
      typedef T * Retval_T;

      static T * convert(VALUE x);
    };

    template<typename T>
    struct from_ruby_<T const *>
    {
      typedef T const * Retval_T;

      static T const * convert(VALUE x);
    };

    template<typename T>
    struct from_ruby_<T &>
    {
      typedef T & Retval_T;

      static T & convert(VALUE x);
    };
  } // detail
} // Rice

#endif // Rice__detail__from_ruby__hpp_

