#ifndef Rice__detail__from_ruby__ipp_
#define Rice__detail__from_ruby__ipp_

#include <stdexcept>

/* This file implements conversions from Ruby to native values fo fundamental types 
   such as bool, int, float, etc. It also includes conversions for chars and strings */
namespace Rice
{
  namespace detail
  {
    template<>
    struct From_Ruby<short>
    {
      static short convert(VALUE x)
      {
        return NUM2SHORT(x);
      }
    };

    template<>
    struct From_Ruby<int>
    {
      static int convert(VALUE x)
      {
        return NUM2INT(x);
      }
    };

    template<>
    struct From_Ruby<long>
    {
      static long convert(VALUE x)
      {
        return (long)NUM2LONG(x);
      }
    };

    template<>
    struct From_Ruby<long long>
    {
      static long long convert(VALUE x)
      {
        return RB_NUM2LL(x);
      }
    };

    template<>
    struct From_Ruby<unsigned short>
    {
      static unsigned short convert(VALUE x)
      {
        return NUM2USHORT(x);
      }
    };

    template<>
    struct From_Ruby<unsigned int>
    {
      static unsigned int convert(VALUE x)
      {
        return NUM2UINT(x);
      }
    };

    template<>
    struct From_Ruby<unsigned long>
    {
      static unsigned long convert(VALUE x)
      {
        return (unsigned long)RB_NUM2ULONG(x);
      }
    };

    template<>
    struct From_Ruby<unsigned long long>
    {
      static unsigned long long convert(VALUE x)
      {
        return RB_NUM2ULL(x);
      }
    };

    template<>
    struct From_Ruby<bool>
    {
      static bool convert(VALUE x)
      {
        return RTEST(x);
      }
    };

    template<>
    struct From_Ruby<char>
    {
      static char convert(VALUE x)
      {
        switch (rb_type(x))
        {
          case T_STRING:
          {
            if (RSTRING_LEN(x) == 1)
            {
              return RSTRING_PTR(x)[0];
            }
            else
            {
              throw std::invalid_argument("from_ruby<char>: string must have length 1");
            }
            break;
          }
          case T_FIXNUM:
          {
            return From_Ruby<long>::convert(x) & 0xff;
            break;
          }
          default:
          {
            throw std::invalid_argument("Cannot convert Ruby value to char");
          }
        }
      }
    };

    template<>
    struct From_Ruby<char*>
    {
      static char const* convert(VALUE x)
      {
        if (x == Qnil)
        {
          return nullptr;
        }
        else
        {
          return RSTRING_PTR(x);
        }
      }
    };

    template<>
    struct From_Ruby<char const*>
    {
      static char const* convert(VALUE x)
      {
        return RSTRING_PTR(x);
      }
    };

    template<>
    struct From_Ruby<float>
    {
      static float convert(VALUE x)
      {
        return (float)rb_num2dbl(x);
      }
    };

    template<>
    struct From_Ruby<double>
    {
      static double convert(VALUE x)
      {
        return rb_num2dbl(x);
      }
    };

    template<>
    struct From_Ruby<std::string>
    {
      static std::string convert(VALUE x)
      {
        return std::string(RSTRING_PTR(x), RSTRING_LEN(x));
      }
    };
  }
}
#endif // Rice__detail__from_ruby__ipp_