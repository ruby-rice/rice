#include "protect.hpp"
#include "Object.hpp"
#include "detail/from_ruby.ipp"
#include "detail/to_ruby.ipp"

// ---------------------------------------------------------------------
/*template<>
inline
VALUE to_ruby<VALUE>(VALUE const& x)
{
  return x;
}*/

template<>
inline
Rice::Object from_ruby<Rice::Object>(VALUE x)
{
  return Rice::Object(x);
}

template<>
inline
VALUE to_ruby<Rice::Object>(Rice::Object const & x)
{
  return x;
}

template<>
inline
short from_ruby<short>(VALUE x)
{
  return NUM2SHORT(x);
}

template<>
inline
VALUE to_ruby<short>(short const & x)
{
  return INT2NUM(x);
}

template<>
inline
int from_ruby<int>(VALUE x)
{
  return NUM2INT(x);
}

template<>
inline
VALUE to_ruby<int>(int const & x)
{
  return INT2NUM(x);
}

template<>
inline
long from_ruby<long>(VALUE x)
{
  return (long)NUM2LONG(x);
}

template<>
inline
VALUE to_ruby<long>(long const & x)
{
  return LONG2NUM(x);
}

template<>
inline
long long from_ruby<long long>(VALUE x)
{
  return RB_NUM2LL(x);
}

template<>
inline
VALUE to_ruby<long long>(long long const & x)
{
  return LL2NUM(x);
}

template<>
inline
unsigned short from_ruby<unsigned short>(VALUE x)
{
  return NUM2USHORT(x);
}

template<>
inline
VALUE to_ruby<unsigned short>(unsigned short const & x)
{
  return UINT2NUM(x);
}

template<>
inline
unsigned int from_ruby<unsigned int>(VALUE x)
{
  return NUM2UINT(x);
}

template<>
inline
VALUE to_ruby<unsigned int>(unsigned int const & x)
{
  return UINT2NUM(x);
}

template<>
inline
unsigned long from_ruby<unsigned long>(VALUE x)
{
  return (unsigned long)RB_NUM2ULONG(x);
}

template<>
inline
VALUE to_ruby<unsigned long>(unsigned long const & x)
{
  return ULONG2NUM(x);
}

template<>
inline
unsigned long long from_ruby<unsigned long long>(VALUE x)
{
  return RB_NUM2ULL(x);
}

template<>
inline
VALUE to_ruby<unsigned long long>(unsigned long long const & x)
{
  return ULL2NUM(x);
}

// ---------------------------------------------------------------------
template<>
inline
bool from_ruby<bool>(VALUE x)
{
  return RTEST(x);
}

template<>
inline
VALUE to_ruby<bool>(bool const & x)
{
  return x ? Qtrue : Qfalse;
}

// ---------------------------------------------------------------------
template<>
inline
char from_ruby<char>(VALUE x)
{
  if(rb_type(x) == T_STRING)
  {
    if(RSTRING_LEN(x) == 1)
    {
      return RSTRING_PTR(x)[0];
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
VALUE to_ruby<char>(char const & x)
{
  return to_ruby<int>(x);
}

// ---------------------------------------------------------------------

template<>
inline
unsigned char from_ruby<unsigned char>(VALUE x)
{
  // TODO: I'm not sure if this is the right behavior yet
  return from_ruby<char>(x);
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
float from_ruby<float>(VALUE x)
{
  return (float)rb_num2dbl(x);
}

template<>
inline
VALUE to_ruby<float>(float const & x)
{
  return Rice::protect(Rice::detail::dbl2num, x);
}

// ---------------------------------------------------------------------

template<>
inline
double from_ruby<double>(VALUE x)
{
  return rb_num2dbl(x);
}

template<>
inline
VALUE to_ruby<double>(double const & x)
{
  return Rice::detail::dbl2num(x);
}

// ---------------------------------------------------------------------
template<>
inline
char const * from_ruby<char const *>(VALUE x)
{
  return RSTRING_PTR(x);
}

template<>
inline
VALUE to_ruby<char const *>(char const * const & x)
{
  return Rice::protect(rb_str_new2, x);
}

// ---------------------------------------------------------------------
template<>
inline
std::string from_ruby<std::string>(VALUE x)
{
  return std::string(RSTRING_PTR(x), RSTRING_LEN(x));
}

template<>
inline
VALUE to_ruby<std::string>(std::string const & x)
{
  return Rice::protect(rb_str_new, x.data(), (long)x.size());
}

template<>
inline
std::string* from_ruby<std::string* >(VALUE x)
{
  return new std::string(Rice::String(x).str());
}
