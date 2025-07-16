#include <variant>

namespace Rice::detail
{
  template<>
  struct Type<std::monostate>
  {
    constexpr static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<std::monostate>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(const std::monostate& _)
    {
      return Qnil;
    }


  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class To_Ruby<std::monostate&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(const std::monostate& data)
    {
      return Qnil;
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class From_Ruby<std::monostate>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
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
  };

  template<>
  class From_Ruby<std::monostate&>
  {
  public:
    From_Ruby() = default;

    explicit From_Ruby(Arg* arg)
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
    std::monostate converted_ = std::monostate();
  };
}
