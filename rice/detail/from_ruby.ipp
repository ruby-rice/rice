#ifndef Rice__detail__from_ruby__ipp_
#define Rice__detail__from_ruby__ipp_

#include "../Data_Type.hpp"
#include <typeinfo>

#if 0
namespace Rice
{
  namespace detail
  {
    template<typename T>
    struct unpointer
    {
      // can't unpointer a non-pointer
    };

    template<typename T>
    struct unpointer<T *>
    {
      typedef T type;
    };
  }
}
#endif

template<typename T>
T Rice::detail::from_ruby_<T>::
convert(Rice::Object x)
{
  if(rb_type(x.value()) == T_DATA)
  {
    return *Data_Type<T>::from_ruby(x);
  }
  else
  {
    std::string s("No conversion defined for ");
    s += typeid(T).name();
    throw std::runtime_error(s.c_str());
  }
}

template<typename T>
T * Rice::detail::from_ruby_<T *>::
convert(Rice::Object x)
{
  if(rb_type(x.value()) == T_DATA)
  {
    return Data_Type<T>::from_ruby(x);
  }
  else
  {
    std::string s("No conversion defined for ");
    s += typeid(T).name();
    throw std::runtime_error(s.c_str());
  }
}

template<typename T>
T const * Rice::detail::from_ruby_<T const *>::
convert(Rice::Object x)
{
  return from_ruby<T *>(x);
}

template<typename T>
T & Rice::detail::from_ruby_<T &>::
convert(Rice::Object x)
{
  return *from_ruby<T *>(x);
}

#endif // Rice__detail__from_ruby__ipp_

