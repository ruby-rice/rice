namespace Rice
{
  inline String::String() : Builtin_Object<T_STRING>(detail::protect(rb_str_new2, ""))
  {
  }

  inline String::String(VALUE v) : Builtin_Object<T_STRING>(v)
  {
  }

  inline String::String(Object v) : Builtin_Object<T_STRING>(v)
  {
  }

  inline String::String(char const* s) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new_cstr, s))
  {
  }

  inline String::String(std::string const& s) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new, s.data(), (long)s.length()))
  {
  }

  inline String::String(std::string_view const& s) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new, s.data(), (long)s.length()))
  {
  }

  inline String::String(Identifier id) : Builtin_Object<T_STRING>(detail::protect(rb_utf8_str_new_cstr, id.c_str()))
  {
  }

  template <typename... Arg_Ts>
  inline String String::format(char const* fmt, Arg_Ts&&...args)
  {
    size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
    std::string temp(size, '\0');

    // size+1 avoids truncating the string. Otherwise snprintf writes n - 1 characters
    // to allow space for null character but we don't need that since std::string
    // will add a null character internally at n + 1
    std::snprintf(&temp[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

    String s = String(temp.c_str());
    return s;
  }

  inline size_t String::length() const
  {
    return RSTRING_LEN(value());
  }

  inline char String::operator[](ptrdiff_t index) const
  {
    return RSTRING_PTR(value())[index];
  }

  inline char const* String::c_str() const
  {
    return RSTRING_PTR(value());
  }

  inline std::string String::str() const
  {
    return std::string(RSTRING_PTR(value()), length());
  }

  template<typename T>
  inline Array String::unpack() const
  {
    return this->call("unpack", detail::RubyType<T>::packTemplate.c_str());
  }

  inline Identifier String::intern() const
  {
    return rb_intern(c_str());
  }
}

namespace Rice::detail
{
  template<>
  struct Type<String>
  {
    static bool verify()
    {
      return true;
    }
  };
  
  template<>
  class To_Ruby<String>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo)
    {
    }

    VALUE convert(String const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<String>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      switch (rb_type(value))
      {
        case RUBY_T_STRING:
          return Convertible::Exact;
          break;
        default:
          return Convertible::None;
      }
    }

    String convert(VALUE value)
    {
      return String(value);
    }
  };
}