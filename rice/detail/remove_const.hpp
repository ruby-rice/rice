#ifndef Rice__detail__remove_const__hpp_
#define Rice__detail__remove_const__hpp_

namespace Rice
{

namespace detail
{

template<typename T>
struct remove_const { typedef T Type; };

template<typename T>
struct remove_const<T const> { typedef T Type; };

} // namespace detail

} // namespace Rice

#endif // Rice__detail__remove_const__hpp_

