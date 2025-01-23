
namespace Rice
{
  namespace detail
  {
    template<>
    class To_Ruby<void>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const void*)
      {
        throw std::runtime_error("Converting from void pointer is not implemented");
        return Qnil;
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<std::nullptr_t>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(std::nullptr_t const)
      {
        return Qnil;
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<short>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<short&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<int>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

      VALUE convert(const volatile int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<int&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_int2num_inline, (int)native);
#else
        return RB_INT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long& native)
      {
        return protect(rb_long2num_inline, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<long long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<long long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const long long& native)
      {
        return protect(rb_ll2inum, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned short>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned short&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned short& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned int>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned int&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned int& native)
      {
#ifdef rb_int2num_inline
        return protect(rb_uint2num_inline, (unsigned int)native);
#else
        return RB_UINT2NUM(native);
#endif
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ulong2num_inline, native);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

      VALUE convert(const volatile unsigned long long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }
    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned long long&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned long long& native)
      {
        if (this->arg_ && this->arg_->isValue())
        {
          return native;
        }
        else
        {
          return protect(rb_ull2inum, (unsigned long long)native);
        }
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<float>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<float&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const float& native)
      {
        return protect(rb_float_new, (double)native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<double>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<double&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const double& native)
      {
        return protect(rb_float_new, native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<bool>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<bool&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const bool& native)
      {
        return native ? Qtrue : Qfalse;
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<char>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<char&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const char& native)
      {
        return To_Ruby<int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<char*>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
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
        else if (this->arg_ && this->arg_->isOwner())
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
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<char[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

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

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned char>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<unsigned char&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const unsigned char& native)
      {
        return To_Ruby<unsigned int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<signed char>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<>
    class To_Ruby<signed char&>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(const signed char& native)
      {
        return To_Ruby<signed int>().convert(native);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template <>
    class To_Ruby<void*>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(void* data)
      {
        if (this->arg_ && this->arg_->isOpaque())
        {
          return VALUE(data);
        }
        else if (data)
        {
          // Note that T could be a pointer or reference to a base class while data is in fact a
          // child class. Lookup the correct type so we return an instance of the correct Ruby class
          std::pair<VALUE, rb_data_type_t*> rubyTypeInfo = detail::Registries::instance.types.figureType(data);
          bool isOwner = this->arg_ && this->arg_->isOwner();
          return detail::wrap(rubyTypeInfo.first, rubyTypeInfo.second, data, isOwner);
        }
        else
        {
          return Qnil;
        }
      }

      VALUE convert(const void* data)
      {
        return convert((void*)data);
      }

    private:
      Arg* arg_ = nullptr;
    };
  }
}