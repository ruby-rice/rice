RICE_DETAIL_BEGIN_NAMESPACE
  template<>
  struct Type<bool>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cTrueClass;
    }
  };

  template<>
  struct Type<char>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<int N>
  struct Type<char[N]>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<char*>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<signed char>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<int N>
  struct Type<signed char[N]>
  {
    static bool verify()
    {
      define_buffer<signed char>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned char>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<int N>
  struct Type<unsigned char[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned char>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  struct Type<short>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<short[N]>
  {
    static bool verify()
    {
      define_buffer<short>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  struct Type<unsigned short>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned short[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned short>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<int>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<int[N]>
  {
    static bool verify()
    {
      define_buffer<int>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned int>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned int[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned int>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<long[N]>
  {
    static bool verify()
    {
      define_buffer<long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned long[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };
  
  template<>
  struct Type<long long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<long long[N]>
  {
    static bool verify()
    {
      define_buffer<long long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<unsigned long long>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cInteger;
    }
  };

  template<int N>
  struct Type<unsigned long long[N]>
  {
    static bool verify()
    {
      define_buffer<unsigned long long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<float>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cFloat;
    }
  };

  template<int N>
  struct Type<float[N]>
  {
    static bool verify()
    {
      define_buffer<float>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<double>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cFloat;
    }
  };

  template<int N>
  struct Type<double[N]>
  {
    static bool verify()
    {
      define_buffer<double>();
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cString;
    }
  };

  template<>
  struct Type<void>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cNilClass;
    }
  };

  template<>
  struct Type<void*>
  {
    static bool verify()
    {
      Type<Pointer<void>>::verify();
      return true;
    }
  };
RICE_DETAIL_END_NAMESPACE
