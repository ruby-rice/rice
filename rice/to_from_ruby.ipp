#include "protect.hpp"
#include "Object.hpp"
#include "detail/from_ruby.ipp"
#include "detail/to_ruby.ipp"

// ---------------------------------------------------------------------
template<>
inline
VALUE to_ruby<Rice::Object>(Rice::Object const & x)
{
  return x;
}

template<>
inline
VALUE to_ruby<short>(short const & x)
{
  return INT2NUM(x);
}

template<>
inline
VALUE to_ruby<int>(int const & x)
{
  return INT2NUM(x);
}

template<>
inline
VALUE to_ruby<long>(long const & x)
{
  return LONG2NUM(x);
}

template<>
inline
VALUE to_ruby<long long>(long long const & x)
{
  return LL2NUM(x);
}

template<>
inline
VALUE to_ruby<unsigned short>(unsigned short const & x)
{
  return UINT2NUM(x);
}

template<>
inline
VALUE to_ruby<unsigned int>(unsigned int const & x)
{
  return UINT2NUM(x);
}

template<>
inline
VALUE to_ruby<unsigned long>(unsigned long const & x)
{
  return ULONG2NUM(x);
}

template<>
inline
VALUE to_ruby<unsigned long long>(unsigned long long const & x)
{
  return ULL2NUM(x);
}

template<>
inline
VALUE to_ruby<bool>(bool const & x)
{
  return x ? Qtrue : Qfalse;
}

template<>
inline
VALUE to_ruby<char>(char const & x)
{
  return to_ruby<int>(x);
}

template<>
inline
VALUE to_ruby<unsigned char>(unsigned char const & x)
{
  return to_ruby<unsigned int>(x);
}

// ---------------------------------------------------------------------
namespace Rice
{
  namespace detail
  {
    inline VALUE dbl2num(double x)
    {
      return rb_float_new(x);
    }
  }
}

template<>
inline
VALUE to_ruby<float>(float const & x)
{
  return Rice::protect(Rice::detail::dbl2num, x);
}

template<>
inline
VALUE to_ruby<double>(double const & x)
{
  return Rice::detail::dbl2num(x);
}

template<>
inline
VALUE to_ruby<char const *>(char const * const & x)
{
  return Rice::protect(rb_str_new2, x);
}

template<>
inline
VALUE to_ruby<std::string>(std::string const & x)
{
  return Rice::protect(rb_str_new, x.data(), (long)x.size());
}