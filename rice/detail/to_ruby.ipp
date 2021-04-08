
namespace Rice
{
  namespace detail
  {
    // This template is never called but needs to exist for MSVC to successfully compile
    // this line from Wrapped_Function.ipp:
    // 
    //   return To_Ruby<Return_T>::convert(result);
    // 
    // Note there is a if constexpr check for Return_T being void. Sigh.
    template<>
    struct To_Ruby<void>
    {
      static VALUE convert(void*, bool takeOwnership = false)
      {
        return Qnil;
      }
    };

    template<>
    struct To_Ruby<std::nullptr_t>
    {
      static VALUE convert(std::nullptr_t, bool takeOwnership = false)
      {
        return Qnil;
      }
    };

    template<>
    struct To_Ruby<short>
    {
      static VALUE convert(short const& x, bool takeOwnership = false)
      {
        return INT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<int>
    {
      static VALUE convert(int const& x, bool takeOwnership = false)
      {
        return INT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<long>
    {
      static VALUE convert(long const& x, bool takeOwnership = false)
      {
        return LONG2NUM(x);
      }
    };

    template<>
    struct To_Ruby<long long>
    {
      static VALUE convert(long long const& x, bool takeOwnership = false)
      {
        return LL2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned short>
    {
      static VALUE convert(unsigned short const& x, bool takeOwnership = false)
      {
        return UINT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned int>
    {
      static VALUE convert(unsigned int const& x, bool takeOwnership = false)
      {
        return UINT2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned long>
    {
      static VALUE convert(unsigned long const& x, bool takeOwnership = false)
      {
        return ULONG2NUM(x);
      }
    };

    template<>
    struct To_Ruby<unsigned long long>
    {
      static VALUE convert(unsigned long long const& x, bool takeOwnership = false)
      {
        return ULL2NUM(x);
      }
    };

    template<>
    struct To_Ruby<float>
    {
      static VALUE convert(float const& x, bool takeOwnership = false)
      {
        return rb_float_new(x);
      }
    };

    template<>
    struct To_Ruby<double>
    {
      static VALUE convert(double const& x, bool takeOwnership = false)
      {
        return rb_float_new(x);
      }
    };

    template<>
    struct To_Ruby<bool>
    {
      static VALUE convert(bool const& x, bool takeOwnership = false)
      {
        return x ? Qtrue : Qfalse;
      }
    };

    template<>
    struct To_Ruby<char>
    {
      static VALUE convert(char const& x, bool takeOwnership = false)
      {
        return To_Ruby<int>::convert(x, takeOwnership);
      }
    };

    template<>
    struct To_Ruby<unsigned char>
    {
      static VALUE convert(unsigned char const& x, bool takeOwnership = false)
      {
        return To_Ruby<unsigned int>::convert(x, takeOwnership);
      }
    };

    template<>
    struct To_Ruby<const char*>
    {
      static VALUE convert(const char* x, bool takeOwnership = false)
      {
        return rb_str_new2(x);
      }
    };
  }
}