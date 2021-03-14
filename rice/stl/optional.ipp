#include "../detail/to_ruby.hpp"

namespace Rice::detail
{
  template<typename T>
  struct From_Ruby<std::optional<T>>
  {
    static std::optional<T> convert(VALUE x)
    {
      if (NIL_P(x)) {
        return std::nullopt;
      } else {
        return From_Ruby<T>::convert(x);
      }
    }
  };

  template<typename T>
  struct To_Ruby<std::optional<T>>
  {
    static VALUE convert(std::optional<T> const & x, bool takeOwnership = false)
    {
      if (x.has_value()) {
        return To_Ruby<T>::convert(*x, takeOwnership);
      } else {
        return Qnil;
      }
    }
  };
}
