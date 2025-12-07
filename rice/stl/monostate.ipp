#include <variant>

RICE_DETAIL_BEGIN_NAMESPACE
  template<>
  struct Type<std::monostate>
  {
    constexpr static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cNilClass;
    }
  };

  template<>
  class To_Ruby<std::monostate>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::monostate&)
    {
      return Qnil;
    }


  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class To_Ruby<std::monostate&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Arg* arg) : arg_(arg)
    {
    }

    VALUE convert(const std::monostate&)
    {
      return Qnil;
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<std::monostate>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return value == Qnil ? Convertible::Exact : Convertible::None;
    }

    std::monostate convert(VALUE value)
    {
      if (value == Qnil)
      {
        return std::monostate();
      }
      else
      {
        throw std::runtime_error("Can only convert nil values to std::monostate");
      }
    }

  private:
    Arg* arg_ = nullptr;
  };

  template<>
  class From_Ruby<std::monostate&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg) : arg_(arg)
    {
    }

    Convertible is_convertible(VALUE value)
    {
      return value == Qnil ? Convertible::Exact : Convertible::None;
    }

    std::monostate& convert(VALUE value)
    {
      if (value == Qnil)
      {
        return this->converted_;
      }
      else
      {
        throw std::runtime_error("Can only convert nil values to std::monostate");
      }
    }
    
  private:
    Arg* arg_ = nullptr;
    std::monostate converted_ = std::monostate();
  };
RICE_DETAIL_END_NAMESPACE
