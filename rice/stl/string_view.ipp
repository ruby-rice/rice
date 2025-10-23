#include <string_view>

namespace Rice::detail
{
  template<>
  struct Type<std::string_view>
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
  class To_Ruby<std::string_view>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(std::string_view const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class To_Ruby<std::string_view&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(std::string_view const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<std::string_view>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
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

    std::string_view convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      return std::string_view(RSTRING_PTR(value), RSTRING_LEN(value));
    }

  private:
    Arg* arg_ = nullptr;
  };
}