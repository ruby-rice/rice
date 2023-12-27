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
    VALUE convert(std::monostate& _)
    {
      return Qnil;
    }
  };

  template<>
  class To_Ruby<std::monostate&>
  {
  public:
    static VALUE convert(std::monostate& data, bool takeOwnership = false)
    {
      return Qnil;
    }
  };

  template<>
  class From_Ruby<std::monostate>
  {
  public:
    bool is_convertible(VALUE value)
    {
      return false;
    }

    std::monostate convert(VALUE value)
    {
      return std::monostate();
    }
  };

  template<>
  class From_Ruby<std::monostate&>
  {
  public:
    bool is_convertible(VALUE value)
    {
      return false;
    }

    std::monostate& convert(VALUE value)
    {
      return this->converted_;
    }
    
  private:
    std::monostate converted_ = std::monostate();
  };
}