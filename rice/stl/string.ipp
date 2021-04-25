#include <string>
#include "../detail/ruby.hpp"
#include "../detail/Type.hpp"
#include "../detail/from_ruby.hpp"
#include "../detail/to_ruby.hpp"

namespace Rice::detail
{
  template <>
  struct is_builtin<std::string> : public std::true_type {};

  template<>
  struct To_Ruby<std::string>
  {
    VALUE convert(std::string const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  struct To_Ruby<std::string&>
  {
    VALUE convert(std::string const& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class From_Ruby<std::string>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(std::string defaultValue) : defaultValue_(defaultValue)
    {
    }

    std::string convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        return std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      }
    }

  private:
    std::optional<std::string> defaultValue_;
  };

  template<>
  class From_Ruby<std::string*>
  {
  public:
    std::string* convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return &this->converted_;
    }

  private:
    std::string converted_;
  };

  template<>
  class From_Ruby<std::string&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(std::string defaultValue) : defaultValue_(defaultValue)
    {
    }

    std::string& convert(VALUE value)
    {
      if (value == Qnil && this->defaultValue_)
      {
        return this->defaultValue_.value();
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
        return this->converted_;
      }
    }

  private:
    std::optional<std::string> defaultValue_;
    std::string converted_;
  };
}