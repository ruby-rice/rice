#include <string>

namespace Rice::detail
{
  template<>
  struct Type<std::string>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<std::string>
  {
  public:
    VALUE convert(const std::string& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class To_Ruby<std::string&>
  {
  public:
    VALUE convert(const std::string& x)
    {
      return detail::protect(rb_external_str_new, x.data(), (long)x.size());
    }
  };

  template<>
  class To_Ruby<std::string*>
  {
  public:
    VALUE convert(const std::string* x)
    {
      return detail::protect(rb_external_str_new, x->data(), (long)x->size());
    }
  };

  template<>
  class To_Ruby<std::string*&>
  {
  public:
    VALUE convert(const std::string* x)
    {
      return detail::protect(rb_external_str_new, x->data(), (long)x->size());
    }
  };

  template<>
  class From_Ruby<std::string>
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

    std::string convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<std::string>();
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        return std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<std::string&>
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

    std::string& convert(VALUE value)
    {
      if (value == Qnil && this->arg_ && this->arg_->hasDefaultValue())
      {
        return this->arg_->defaultValue<std::string>();
      }
      else
      {
        detail::protect(rb_check_type, value, (int)T_STRING);
        this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
        return this->converted_;
      }
    }

  private:
    Arg* arg_ = nullptr;
    std::string converted_ = "";
  };

  template<>
  class From_Ruby<std::string*>
  {
  public:
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
  class From_Ruby<std::string*&>
  {
  public:
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

    std::string* convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      this->converted_ = std::string(RSTRING_PTR(value), RSTRING_LEN(value));
      return &this->converted_;
    }

  private:
    std::string converted_;
  };
}