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
  };

  template<>
  class To_Ruby<std::string_view>
  {
  public:
    VALUE convert(std::string_view const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class To_Ruby<std::string_view&>
  {
  public:
    VALUE convert(std::string_view const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
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
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<std::string_view>();
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        return std::string_view(RSTRING_PTR(value), RSTRING_LEN(value));
      }
    }

  private:
    Arg* arg_ = nullptr;
  };
}