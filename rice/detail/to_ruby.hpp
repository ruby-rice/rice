#ifndef Rice__detail__to_ruby__hpp_
#define Rice__detail__to_ruby__hpp_

namespace Rice
{
  namespace detail
  {
    template<typename T>
    struct to_ruby_
    {
      static Rice::Object convert(T const & x);
    };
  } // detail
} // Rice

#endif // Rice__detail__to_ruby__hpp_
