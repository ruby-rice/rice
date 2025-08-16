namespace Rice::detail
{
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
  struct Type<bool*>
  {
    static bool verify()
    {
      define_buffer<bool>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<bool>>();
      return pair.first;
    }
  };

  template<>
  struct Type<char**>
  {
    static bool verify()
    {
      define_buffer<char*>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<char*>>();
      return pair.first;
    }
  };

  template<>
  struct Type<unsigned char*>
  {
    static bool verify()
    {
      define_buffer<unsigned char>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<unsigned char>>();
      return pair.first;
    }
  };

  template<>
  struct Type<unsigned char**>
  {
    static bool verify()
    {
      define_buffer<unsigned char*>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<unsigned char*>>();
      return pair.first;
    }
  };

  template<>
  struct Type<signed char*>
  {
    static bool verify()
    {
      define_buffer<signed char>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<signed char>>();
      return pair.first;
    }
  };

  template<>
  struct Type<short*>
  {
    static bool verify()
    {
      define_buffer<short>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<short>>();
      return pair.first;
    }
  };

  template<>
  struct Type<unsigned short*>
  {
    static bool verify()
    {
      define_buffer<unsigned short>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<unsigned short>>();
      return pair.first;
    }
  };

  template<>
  struct Type<int*>
  {
    static bool verify()
    {
      define_buffer<int>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<int>>();
      return pair.first;
    }
  };

  template<>
  struct Type<unsigned int*>
  {
    static bool verify()
    {
      define_buffer<unsigned int>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<unsigned int>>();
      return pair.first;
    }
  };

  template<>
  struct Type<long*>
  {
    static bool verify()
    {
      define_buffer<long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<long>>();
      return pair.first;
    }
  };

  template<>
  struct Type<unsigned long*>
  {
    static bool verify()
    {
      define_buffer<unsigned long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<unsigned long>>();
      return pair.first;
    }
  };

  template<>
  struct Type<long long*>
  {
    static bool verify()
    {
      define_buffer<long long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<long long>>();
      return pair.first;
    }
  };

  template<>
  struct Type<unsigned long long*>
  {
    static bool verify()
    {
      define_buffer<unsigned long long>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<unsigned long long>>();
      return pair.first;
    }
  };

  template<>
  struct Type<float*>
  {
    static bool verify()
    {
      define_buffer<float>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<float>>();
      return pair.first;
    }
  };

  template<>
  struct Type<double*>
  {
    static bool verify()
    {
      define_buffer<double>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<double>>();
      return pair.first;
    }
  };

  template<>
  struct Type<void*>
  {
    static bool verify()
    {
      define_buffer<void>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<void>>();
      return pair.first;
    }
  };

  template<typename T>
  struct Type<T**>
  {
    static bool verify()
    {
      define_buffer<T*>();
      return true;
    }

    static VALUE rubyKlass()
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Buffer<T*>>();
      return pair.first;
    }
  };
}