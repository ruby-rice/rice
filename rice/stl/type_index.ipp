#include <typeindex>

namespace Rice::detail
{
  template<>
  struct Type<std::type_index>
  {
    constexpr static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<std::type_index>
  {
  public:
    VALUE convert(const std::type_index& _)
    {
      throw std::runtime_error("std::type_index support is not yet implemented");
      return Qnil;
    }
  };

  template<>
  class To_Ruby<std::type_index&>
  {
  public:
    static VALUE convert(const std::type_index& data, bool takeOwnership = false)
    {
      throw std::runtime_error("std::type_index support is not yet implemented");
      return Qnil;
    }
  };

  template<>
  class From_Ruby<std::type_index&>
  {
  public:
    Convertible is_convertible(VALUE value)
    {
      return Convertible::None;
    }

    std::type_index& convert(VALUE value)
    {
      throw std::runtime_error("std::type_index support is not yet implemented");
    }
  };
}
