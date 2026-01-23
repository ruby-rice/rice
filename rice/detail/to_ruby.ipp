
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

    template<bool N>
    class To_Ruby<bool[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(bool data[N])
      {
        Buffer<bool> buffer(data, N);
        Data_Object<Buffer<bool>> dataObject(std::move(buffer));
        return dataObject.value();
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

    template<int N>
    class To_Ruby<int[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(int data[N])
      {
        Buffer<int> buffer(data, N);
        Data_Object<Buffer<int>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<unsigned int[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned int data[N])
      {
        Buffer<unsigned int> buffer(data, N);
        Data_Object<Buffer<unsigned int>> dataObject(std::move(buffer));
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

      VALUE convert(const char* data)
      {
        if (!data)
        {
          return Qnil;
        }
        else if (strlen(data) > 0 && data[0] == ':')
        {
          size_t symbolLength = strlen(data) - 1;
          char* symbol = new char[symbolLength];
          strncpy(symbol, data + 1, symbolLength);
          ID id = protect(rb_intern2, symbol, (long)symbolLength);
          delete[] symbol;
          return protect(rb_id2sym, id);
        }
        else if (this->arg_ && this->arg_->isOwner())
        {
          // This copies the buffer but does not free it
          VALUE result = protect(rb_usascii_str_new_cstr, data);
          // And free the char* since we were told to take "ownership"
          // TODO - is this a good idea?
          //free(data);
          return result;
        }
        else
        {
          // Does NOT copy the passed in buffer and does NOT free it when the string is GCed
          long len = (long)strlen(data);
          VALUE result = protect(rb_str_new_static, data, len);
          // Freeze the object so Ruby can't modify the C string
          return rb_obj_freeze(result);
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

    template<int N>
    class To_Ruby<unsigned char[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned char data[N])
      {
        Buffer<unsigned char> buffer(data, N);
        Data_Object<Buffer<unsigned char>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<signed char[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(signed char data[N])
      {
        Buffer<signed char> buffer(data, N);
        Data_Object<Buffer<signed char>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<double[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(double data[N])
      {
        Buffer<double> buffer(data, N);
        Data_Object<Buffer<double>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<float[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(float data[N])
      {
        Buffer<float> buffer(data, N);
        Data_Object<Buffer<float>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(long data[N])
      {
        Buffer<long> buffer(data, N);
        Data_Object<Buffer<long>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<unsigned long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned long data[N])
      {
        Buffer<unsigned long> buffer(data, N);
        Data_Object<Buffer<unsigned long>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<long long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(long long data[N])
      {
        Buffer<long long> buffer(data, N);
        Data_Object<Buffer<long long>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<unsigned long long[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned long long data[N])
      {
        Buffer<unsigned long long> buffer(data, N);
        Data_Object<Buffer<unsigned long long>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<short[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(short data[N])
      {
        Buffer<short> buffer(data, N);
        Data_Object<Buffer<short>> dataObject(std::move(buffer));
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

    template<int N>
    class To_Ruby<unsigned short[N]>
    {
    public:
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
      }

      VALUE convert(unsigned short data[N])
      {
        Buffer<unsigned short> buffer(data, N);
        Data_Object<Buffer<unsigned short>> dataObject(std::move(buffer));
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
      To_Ruby() = default;

      explicit To_Ruby(Arg* arg) : arg_(arg)
      {
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
          bool isOwner = this->arg_ && this->arg_->isOwner();
          return detail::wrap(Data_Type<Pointer<void>>::klass(), Data_Type<Pointer<void>>::ruby_data_type(), data, isOwner);
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