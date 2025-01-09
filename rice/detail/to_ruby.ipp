
namespace Rice
{
  namespace detail
  {
    template<>
    class To_Ruby<void>
    {
    public:
      VALUE convert(void const*)
      {
        throw std::runtime_error("Converting from void pointer is not implemented");
        return Qnil;
      }
    };

    template<>
    class To_Ruby<std::nullptr_t>
    {
    public:
      VALUE convert(std::nullptr_t const)
      {
        return Qnil;
      }
    };

    template<>
    class To_Ruby<short>
    {
    public:
      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<short&>
    {
    public:
      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<int>
    {
    public:
      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<int&>
    {
    public:
      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<long>
    {
    public:
      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }
    };

    template<>
    class To_Ruby<long&>
    {
    public:
      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }
    };

    template<>
    class To_Ruby<long long>
    {
    public:
      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }
    };

    template<>
    class To_Ruby<long long&>
    {
    public:
      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }
    };

    template<>
    class To_Ruby<unsigned short>
    {
    public:
      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned short&>
    {
    public:
      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned int>
    {
    public:
      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned int&>
    {
    public:
      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }
    };

    template<>
    class To_Ruby<unsigned long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->returnInfo_ && this->returnInfo_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<>
    class To_Ruby<float>
    {
    public:
      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }
    };

    template<>
    class To_Ruby<float&>
    {
    public:
      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }
    };

    template<>
    class To_Ruby<double>
    {
    public:
      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }
    };

    template<>
    class To_Ruby<double&>
    {
    public:
      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }
    };

    template<>
    class To_Ruby<bool>
    {
    public:
      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }
    };

    template<>
    class To_Ruby<bool&>
    {
    public:
      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }
    };

    template<>
    class To_Ruby<char>
    {
    public:
      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }
    };

    template<>
    class To_Ruby<char&>
    {
    public:
      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }
    };

    template<>
    class To_Ruby<unsigned char>
    {
    public:
      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }
    };

    template<>
    class To_Ruby<unsigned char&>
    {
    public:
      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }
    };

    template<>
    class To_Ruby<signed char>
    {
    public:
      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }
    };

    template<>
    class To_Ruby<signed char&>
    {
    public:
      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }
    };

    template<>
    class To_Ruby<char*>
    {
    public:
      VALUE convert(char const* buffer)
      {
        if (!buffer)
        {
          return Qnil;
        }
        else if (strlen(buffer) > 0 && buffer[0] == ':')
        {
          size_t symbolLength = strlen(buffer) - 1;
          char* symbol = new char[symbolLength];
          strncpy(symbol, buffer + 1, symbolLength);
          ID id = protect(rb_intern2, symbol, (long)symbolLength);
          delete[] symbol;
          return protect(rb_id2sym, id);
        }
        else
        {
          return protect(rb_str_new2, buffer);
        }
      }
    };

    template<int N>
    class To_Ruby<char[N]>
    {
    public:
      VALUE convert(char const native[])
      {
        if (N > 0 && native[0] == ':')
        {
          // N count includes a NULL character at the end of the string
          constexpr size_t symbolLength = N - 1;
          char symbol[symbolLength];
          strncpy(symbol, native + 1, symbolLength);
          ID id = protect(rb_intern, symbol);
          return protect(rb_id2sym, id);
        }
        else
        {
          return protect(rb_str_new2, native);
        }
      }
    };
  }
}