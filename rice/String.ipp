inline Rice::String::
String()
  : Builtin_Object<T_STRING>(detail::protect(rb_str_new2, ""))
{
}

inline Rice::String::
String(VALUE v)
  : Builtin_Object<T_STRING>(v)
{
}

inline Rice::String::
String(Object v)
  : Builtin_Object<T_STRING>(v)
{
}

inline Rice::String::
String(char const * s)
  : Builtin_Object<T_STRING>(detail::protect(rb_str_new2, s))
{
}

inline Rice::String::
String(std::string const & s)
  : Builtin_Object<T_STRING>(detail::protect(rb_str_new, s.data(), (long)s.length()))
{
}

inline Rice::String::
String(Identifier id)
  : Builtin_Object<T_STRING>(detail::protect(rb_str_new2, id.c_str()))
{
}

template <typename... Arg_Ts>
inline Rice::String Rice::String::
format(char const * fmt, Arg_Ts&&...args)
{
  size_t size = std::snprintf(nullptr, 0, fmt, std::forward<Arg_Ts>(args)...);
  std::string temp(size, '\0');

  // size+1 avoids trunctaing the string. Otherwise snprintf writes n - 1 characters
  // to allow space for null character but we don't need that since std::string
  // will add a null character internally at n + 1
  std::snprintf(&temp[0], size + 1, fmt, std::forward<Arg_Ts>(args)...);

  String s = String(temp.c_str());
  return s;
}

inline size_t Rice::String::
length() const
{
  return RSTRING_LEN(value());
}

inline char Rice::String::
operator[](ptrdiff_t index) const
{
  return RSTRING_PTR(value())[index];
}

inline char const * Rice::String::
c_str() const
{
  return RSTRING_PTR(value());
}

inline std::string Rice::String::
str() const
{
  return std::string(RSTRING_PTR(value()), length());
}

inline Rice::Identifier Rice::String::
intern() const
{
  return rb_intern(c_str());
}

namespace Rice::detail
{
  template<>
  struct From_Ruby<String>
  {
    static String convert(VALUE value)
    {
      return String(value);
    }
  };

  template<>
  struct To_Ruby<String>
  {
    static VALUE convert(String const& x, bool takeOwnership = false)
    {
      return x.value();
    }
  };
}

