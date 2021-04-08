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
  struct From_Ruby<std::string>
  {
    static std::string convert(VALUE value)
    {
      detail::protect(rb_check_type, value, (int)T_STRING);
      return std::string(RSTRING_PTR(value), RSTRING_LEN(value));
    }
  };

  template<>
  struct To_Ruby<std::string>
  {
    static VALUE convert(std::string const& x, bool takeOwnership = false)
    {
      return detail::protect(rb_str_new, x.data(), (long)x.size());
    }
  };
}