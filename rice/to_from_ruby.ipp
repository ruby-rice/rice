#include "protect.hpp"
#include "Object.hpp"
#include "detail/from_ruby.ipp"
#include "detail/to_ruby.ipp"

// ---------------------------------------------------------------------
template<>
inline
Rice::Object from_ruby<Rice::Object>(Rice::Object x)
{
  return x;
}

template<>
inline
Rice::Object to_ruby<Rice::Object>(Rice::Object const & x)
{
  return x;
}

// ---------------------------------------------------------------------
namespace Rice
{
  namespace detail
  {
    inline int num2int(VALUE x)
    {
      return NUM2INT(x);
    }

    inline VALUE int2num(int x)
    {
      return INT2NUM(x);
    }
  }
}

template<>
inline
int from_ruby<int>(Rice::Object x)
{
  return Rice::protect(Rice::detail::num2int, x);
}

template<>
inline
Rice::Object to_ruby<int>(int const & x)
{
  return Rice::protect(Rice::detail::int2num, x);
}

// ---------------------------------------------------------------------
namespace Rice
{
  namespace detail
  {
    inline long num2long(VALUE x)
    {
      return NUM2LONG(x);
    }

    inline VALUE long2num(long x)
    {
      return LONG2NUM(x);
    }
  }
}

template<>
inline
long from_ruby<long>(Rice::Object x)
{
  return Rice::protect(Rice::detail::num2long, x);
}

template<>
inline
Rice::Object to_ruby<long>(long const & x)
{
  return Rice::protect(Rice::detail::long2num, x);
}

// ---------------------------------------------------------------------
namespace Rice
{
  namespace detail
  {
    inline unsigned int num2uint(VALUE x)
    {
      return NUM2UINT(x);
    }

    inline VALUE uint2num(unsigned int x)
    {
      return UINT2NUM(x);
    }
  }
}

template<>
inline
unsigned int from_ruby<unsigned int>(Rice::Object x)
{
  return Rice::protect(Rice::detail::num2uint, x);
}

template<>
inline
Rice::Object to_ruby<unsigned int>(unsigned int const & x)
{
  return Rice::protect(Rice::detail::uint2num, x);
}

// ---------------------------------------------------------------------
namespace Rice
{
  namespace detail
  {
    inline unsigned long num2ulong(VALUE x)
    {
      return NUM2ULONG(x);
    }

    inline VALUE ulong2num(unsigned long x)
    {
      return ULONG2NUM(x);
    }
  }
}

template<>
inline
unsigned long from_ruby<unsigned long>(Rice::Object x)
{
  return Rice::protect(Rice::detail::num2ulong, x);
}

template<>
inline
Rice::Object to_ruby<unsigned long>(unsigned long const & x)
{
  return Rice::protect(Rice::detail::ulong2num, x);
}

// ---------------------------------------------------------------------
template<>
inline
bool from_ruby<bool>(Rice::Object x)
{
  return RTEST(x);
}

template<>
inline
Rice::Object to_ruby<bool>(bool const & x)
{
  return x ? Qtrue : Qfalse;
}

// ---------------------------------------------------------------------
template<>
inline
char from_ruby<char>(Rice::Object x)
{
  if(x.rb_type() == T_STRING)
  {
    if(RSTRING_LEN(x.value()) == 0)
    {
      return RSTRING_PTR(x.value())[0];
    }
    else
    {
      throw std::invalid_argument("from_ruby<char>: string must have length 1");
    }
  }
  else
  {
    return from_ruby<int>(x) & 0xff;
  }
}

template<>
inline
Rice::Object to_ruby<char>(char const & x)
{
  return to_ruby<int>(x);
}

// ---------------------------------------------------------------------

template<>
inline
unsigned char from_ruby<unsigned char>(Rice::Object x)
{
  // TODO: I'm not sure if this is the right behavior yet
  return from_ruby<char>(x);
}

template<>
inline
Rice::Object to_ruby<unsigned char>(unsigned char const & x)
{
  return to_ruby<unsigned int>(x);
}

// ---------------------------------------------------------------------

namespace Rice
{
  namespace detail
  {
    inline VALUE num2dbl(VALUE x, double * d)
    {
      *d = NUM2DBL(x);
      return Qnil;
    }

    inline VALUE dbl2num(double x)
    {
      return rb_float_new(x);
    }
  }
}

template<>
inline
float from_ruby<float>(Rice::Object x)
{
  double d;
  Rice::protect(Rice::detail::num2dbl, x, &d);
  return float(d);
}

template<>
inline
Rice::Object to_ruby<float>(float const & x)
{
  return Rice::protect(Rice::detail::dbl2num, x);
}

// ---------------------------------------------------------------------

template<>
inline
double from_ruby<double>(Rice::Object x)
{
  double d;
  Rice::protect(Rice::detail::num2dbl, x, &d);
  return d;
}

template<>
inline
Rice::Object to_ruby<double>(double const & x)
{
  return Rice::protect(Rice::detail::dbl2num, x);
}

// ---------------------------------------------------------------------
template<>
inline
char const * from_ruby<char const *>(Rice::Object x)
{
  return Rice::String(x).str().data();
}

template<>
inline
Rice::Object to_ruby<char const *>(char const * const & x)
{
  return Rice::protect(rb_str_new2, x);
}

// ---------------------------------------------------------------------
template<>
inline
std::string from_ruby<std::string>(Rice::Object x)
{
  return Rice::String(x).str();
}

template<>
inline
Rice::Object to_ruby<std::string>(std::string const & x)
{
  return Rice::protect(rb_str_new, x.data(), x.size());
}

template<>
inline
std::string* from_ruby<std::string* >(Rice::Object x)
{
  return new std::string(Rice::String(x).str());
}
