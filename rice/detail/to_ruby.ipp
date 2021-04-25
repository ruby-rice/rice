#include "RubyFunction.hpp"

namespace Rice
{
  namespace detail
  {
    template<>
    struct To_Ruby<void>
    {
      VALUE convert(void const*)
      {
        return Qnil;
      }
    };

    template<>
    struct To_Ruby<std::nullptr_t>
    {
      VALUE convert(std::nullptr_t const)
      {
        return Qnil;
      }
    };

    template<>
    struct To_Ruby<short>
    {
      VALUE convert(short const& x)
      {
        return protect(rb_int2num_inline, (int)x);
      }
    };

    template<>
    struct To_Ruby<short&>
    {
      VALUE convert(short const& x)
      {
        return protect(rb_int2num_inline, (int)x);
      }
    };

    template<>
    struct To_Ruby<int>
    {
      VALUE convert(int const& x)
      {
        return protect(rb_int2num_inline, x);
      }
    };

    template<>
    struct To_Ruby<int&>
    {
      VALUE convert(int const& x)
      {
        return protect(rb_int2num_inline, x);
      }
    };

    template<>
    struct To_Ruby<long>
    {
      VALUE convert(long const& x)
      {
        return protect(rb_long2num_inline, x);
      }
    };

    template<>
    struct To_Ruby<long&>
    {
      VALUE convert(long const& x)
      {
        return protect(rb_long2num_inline, x);
      }
    };

    template<>
    struct To_Ruby<long long>
    {
      VALUE convert(long long const& x)
      {
        return protect(rb_ll2inum, x);
      }
    };

    template<>
    struct To_Ruby<long long&>
    {
      VALUE convert(long long const& x)
      {
        return protect(rb_ll2inum, x);
      }
    };

    template<>
    struct To_Ruby<unsigned short>
    {
      VALUE convert(unsigned short const& x)
      {
        return protect(rb_uint2num_inline, (unsigned int)x);
      }
    };

    template<>
    struct To_Ruby<unsigned short&>
    {
      VALUE convert(unsigned short const& x)
      {
        return protect(rb_uint2num_inline, (unsigned int)x);
      }
    };

    template<>
    struct To_Ruby<unsigned int>
    {
      VALUE convert(unsigned int const& x)
      {
        return protect(rb_uint2num_inline, x);
      }
    };

    template<>
    struct To_Ruby<unsigned int&>
    {
      VALUE convert(unsigned int const& x)
      {
        return protect(rb_uint2num_inline, x);
      }
    };

    template<>
    struct To_Ruby<unsigned long>
    {
      To_Ruby() = default;

      explicit To_Ruby(bool isValue) : isValue_(isValue)
      {
      }

      VALUE convert(unsigned long const& x)
      {
        if (this->isValue_)
        {
          return x;
        }
        else
        {
          return protect(rb_ulong2num_inline, x);
        }
      }

    private:
      bool isValue_ = false;
    };

    template<>
    struct To_Ruby<unsigned long&>
    {
      To_Ruby() = default;

      explicit To_Ruby(bool isValue) : isValue_(isValue)
      {
      }

      VALUE convert(unsigned long const& x)
      {
        if (this->isValue_)
        {
          return x;
        }
        else
        {
          return protect(rb_ulong2num_inline, x);
        }
      }

    private:
      bool isValue_ = false;
    };

    template<>
    struct To_Ruby<unsigned long long>
    {
      To_Ruby() = default;

      explicit To_Ruby(bool isValue) : isValue_(isValue)
      {
      }

      VALUE convert(unsigned long long const& x)
      {
        if (this->isValue_)
        {
          return x;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)x);
        }
      }

    private:
      bool isValue_ = false;
    };

    template<>
    struct To_Ruby<unsigned long long&>
    {
      To_Ruby() = default;

      explicit To_Ruby(bool isValue) : isValue_(isValue)
      {
      }

      VALUE convert(unsigned long long const& x)
      {
        if (this->isValue_)
        {
          return x;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)x);
        }
      }

    private:
      bool isValue_ = false;
    };

    template<>
    struct To_Ruby<float>
    {
      VALUE convert(float const& x)
      {
        return protect(rb_float_new, (double)x);
      }
    };

    template<>
    struct To_Ruby<float&>
    {
      VALUE convert(float const& x)
      {
        return protect(rb_float_new, (double)x);
      }
    };

    template<>
    struct To_Ruby<double>
    {
      VALUE convert(double const& x)
      {
        return protect(rb_float_new, x);
      }
    };

    template<>
    struct To_Ruby<double&>
    {
      VALUE convert(double const& x)
      {
        return protect(rb_float_new, x);
      }
    };

    template<>
    struct To_Ruby<bool>
    {
      VALUE convert(bool const& x)
      {
        return x ? Qtrue : Qfalse;
      }
    };

    template<>
    struct To_Ruby<bool&>
    {
      VALUE convert(bool const& x)
      {
        return x ? Qtrue : Qfalse;
      }
    };

    template<>
    struct To_Ruby<char>
    {
      VALUE convert(char const& x)
      {
        return To_Ruby<int>().convert(x);
      }
    };

    template<>
    struct To_Ruby<char&>
    {
      VALUE convert(char const& x)
      {
        return To_Ruby<int>().convert(x);
      }
    };

    template<>
    struct To_Ruby<unsigned char>
    {
      VALUE convert(unsigned char const& x)
      {
        return To_Ruby<unsigned int>().convert(x);
      }
    };

    template<>
    struct To_Ruby<unsigned char&>
    {
      VALUE convert(unsigned char const& x)
      {
        return To_Ruby<unsigned int>().convert(x);
      }
    };

    template<>
    struct To_Ruby<signed char>
    {
      VALUE convert(signed char const& x)
      {
        return To_Ruby<signed int>().convert(x);
      }
    };

    template<>
    struct To_Ruby<signed char&>
    {
      VALUE convert(signed char const& x)
      {
        return To_Ruby<signed int>().convert(x);
      }
    };

    template<>
    struct To_Ruby<char*>
    {
      VALUE convert(char const* x)
      {
        return protect(rb_str_new2, x);
      }
    };
  }
}