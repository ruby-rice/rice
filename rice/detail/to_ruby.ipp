
namespace Rice
{
  namespace detail
  {
    template<>
    class To_Ruby<void>
    {
    public:
      VALUE convert(const void*)
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
    class To_Ruby<char*>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
      {
      }

      VALUE convert(const char* buffer)
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
        else if (this->returnInfo_ && this->returnInfo_->isOwner())
        {
          // This copies the buffer but does not free it. So Ruby is not really
          // taking ownership of it. But there isn't a Ruby API for creating a string
          // from an existing buffer and later freeing it.
          return protect(rb_usascii_str_new_cstr, buffer);
        }
        else
        {
          // Does NOT copy the passed in buffer and does NOT free it when the string is GCed
          long size = (long)strlen(buffer);
          return protect(rb_usascii_str_new_static, buffer, size);
        }
      }

    private:
      Return* returnInfo_ = nullptr;
    };

    template<int N>
    class To_Ruby<char[N]>
    {
    public:
      VALUE convert(const char buffer[])
      {
        if (N > 0 && buffer[0] == ':')
        {
          // N count includes a NULL character at the end of the string
          constexpr size_t symbolLength = N - 1;
          char symbol[symbolLength];
          strncpy(symbol, buffer + 1, symbolLength);
          ID id = protect(rb_intern, symbol);
          return protect(rb_id2sym, id);
        }
        else
        {
          long size = (long)strlen(buffer);
          return protect(rb_usascii_str_new_static, buffer, size);
        }
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
  }
}