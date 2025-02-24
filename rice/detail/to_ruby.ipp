
namespace Rice
{
  namespace detail
  {
    // ===========  bool  ============
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

    // ===========  int  ============
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
    class To_Ruby<int*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<int>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<int>>::verify();
      }

      VALUE convert(int* buffer)
      {
        PointerView<int> pointerView(buffer);
        Data_Object<PointerView<int>> dataObject(pointerView, true);
        return dataObject.value();
      }

      VALUE convert(const int* buffer)
      {
        return this->convert((int*)buffer);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<int[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<int>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<int>>::verify();
      }

      VALUE convert(int buffer[N])
      {
        PointerView<int> pointerView(buffer, N);
        Data_Object<PointerView<int>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned int  ============
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
    class To_Ruby<unsigned int*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned int>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned int>>::verify();
      }

      VALUE convert(unsigned int* buffer)
      {
        PointerView<unsigned int> pointerView(buffer);
        Data_Object<PointerView<unsigned int>> dataObject(pointerView, true);
        return dataObject.value();
      }

      VALUE convert(const unsigned int* buffer)
      {
        return this->convert((unsigned int*)buffer);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned int[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned int>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned int>>::verify();
      }

      VALUE convert(unsigned int buffer[N])
      {
        PointerView<unsigned int> pointerView(buffer, N);
        Data_Object<PointerView<unsigned int>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  char  ============
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

    // ===========  unsigned char  ============
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
    class To_Ruby<unsigned char*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned char>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned char>>::verify();
      }

      VALUE convert(unsigned char* buffer)
      {
        PointerView<unsigned char> pointerView(buffer);
        Data_Object<PointerView<unsigned char>> dataObject(pointerView, true);
        return dataObject.value();
      }

      VALUE convert(const unsigned char* buffer)
      {
        return this->convert((unsigned char*)buffer);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned char[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned char>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned char>>::verify();
      }

      VALUE convert(unsigned char buffer[N])
      {
        PointerView<unsigned char> pointerView(buffer, N);
        Data_Object<PointerView<unsigned char>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  signed char  ============
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

    template<>
    class To_Ruby<signed char*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<signed char>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<signed char>>::verify();
      }

      VALUE convert(signed char* buffer)
      {
        PointerView<signed char> pointerView(buffer);
        Data_Object<PointerView<signed char>> dataObject(pointerView, true);
        return dataObject.value();
      }

      VALUE convert(const signed char* buffer)
      {
        return this->convert((signed char*)buffer);
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<signed char[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<signed char>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<signed char>>::verify();
      }

      VALUE convert(signed char buffer[N])
      {
        PointerView<signed char> pointerView(buffer, N);
        Data_Object<PointerView<signed char>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  double  ============
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
    class To_Ruby<double*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<double>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<double>>::verify();
      }

      VALUE convert(double* buffer)
      {
        PointerView<double> pointerView(buffer);
        Data_Object<PointerView<double>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<double[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<double>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<double>>::verify();
      }

      VALUE convert(double buffer[N])
      {
        PointerView<double> pointerView(buffer, N);
        Data_Object<PointerView<double>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  float  ============
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
    class To_Ruby<float*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<float>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<float>>::verify();
      }

      VALUE convert(float* buffer)
      {
        PointerView<float> pointerView(buffer);
        Data_Object<PointerView<float>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<float[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<float>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<float>>::verify();
      }

      VALUE convert(float buffer[N])
      {
        PointerView<float> pointerView(buffer, N);
        Data_Object<PointerView<float>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  long  ============
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
    class To_Ruby<long*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<long>>::verify();
      }

      VALUE convert(long* buffer)
      {
        PointerView<long> pointerView(buffer);
        Data_Object<PointerView<long>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<long[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<long>>::verify();
      }

      VALUE convert(long buffer[N])
      {
        PointerView<long> pointerView(buffer, N);
        Data_Object<PointerView<long>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned long  ============
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
    class To_Ruby<unsigned long*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned long>>::verify();
      }

      VALUE convert(unsigned long* buffer)
      {
        PointerView<unsigned long> pointerView(buffer);
        Data_Object<PointerView<unsigned long>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned long[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned long>>::verify();
      }

      VALUE convert(unsigned long buffer[N])
      {
        PointerView<unsigned long> pointerView(buffer, N);
        Data_Object<PointerView<unsigned long>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  long long  ============
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
    class To_Ruby<long long*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<long long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<long long>>::verify();
      }

      VALUE convert(long long* buffer)
      {
        PointerView<long long> pointerView(buffer);
        Data_Object<PointerView<long long>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<long long[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<long long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<long long>>::verify();
      }

      VALUE convert(long long buffer[N])
      {
        PointerView<long long> pointerView(buffer, N);
        Data_Object<PointerView<long long>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned long long  ============
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
    class To_Ruby<unsigned long long*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned long long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned long long>>::verify();
      }

      VALUE convert(unsigned long long* buffer)
      {
        PointerView<unsigned long long> pointerView(buffer);
        Data_Object<PointerView<unsigned long long>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned long long[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned long long>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned long long>>::verify();
      }

      VALUE convert(unsigned long long buffer[N])
      {
        PointerView<unsigned long long> pointerView(buffer, N);
        Data_Object<PointerView<unsigned long long>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  short  ============
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
    class To_Ruby<short*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<short>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<short>>::verify();
      }

      VALUE convert(short* buffer)
      {
        PointerView<short> pointerView(buffer);
        Data_Object<PointerView<short>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<short[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<short>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<short>>::verify();
      }

      VALUE convert(short buffer[N])
      {
        PointerView<short> pointerView(buffer, N);
        Data_Object<PointerView<short>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  unsigned short  ============
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
    class To_Ruby<unsigned short*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned short>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned short>>::verify();
      }

      VALUE convert(unsigned short* buffer)
      {
        PointerView<unsigned short> pointerView(buffer);
        Data_Object<PointerView<unsigned short>> dataObject(pointerView, true);
        return dataObject.value();
      }

    private:
      Arg* arg_ = nullptr;
    };

    template<int N>
    class To_Ruby<unsigned short[N]>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<unsigned short>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<unsigned short>>::verify();
      }

      VALUE convert(unsigned short buffer[N])
      {
        PointerView<unsigned short> pointerView(buffer, N);
        Data_Object<PointerView<unsigned short>> dataObject(pointerView, true);
        return dataObject.value();
      }
    private:
      Arg* arg_ = nullptr;
    };

    // ===========  std::nullptr_t  ============
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

    // ===========  void  ============
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
 
    template <>
    class To_Ruby<void*>
    {
    public:
      To_Ruby()
      {
        detail::Type<PointerView<void>>::verify();
      };

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
        detail::Type<PointerView<void>>::verify();
      }

      VALUE convert(void* data)
      {
        if (data == nullptr)
        {
          return Qnil;
        }
        else if (this->arg_ && this->arg_->isOpaque())
        {
          return VALUE(data);
        }
        else
        {
          PointerView<void> pointerView(data);
          Data_Object<PointerView<void>> dataObject(pointerView, true);
          return dataObject.value();
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