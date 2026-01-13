#include <ios>

namespace Rice
{
  inline Data_Type<std::ios_base> define_ios_base()
  {
    Module rb_mStd = define_module("Std");
    if (Data_Type<std::ios_base>::check_defined("IOSBase", rb_mStd))
    {
      return Data_Type<std::ios_base>();
    }

    Data_Type<std::ios_base> result = define_class_under<std::ios_base>(rb_mStd, "IOSBase");

    result.const_set("GOODBIT", static_cast<int>(std::ios_base::goodbit));
    result.const_set("BADBIT", static_cast<int>(std::ios_base::badbit));
    result.const_set("FAILBIT", static_cast<int>(std::ios_base::failbit));
    result.const_set("EOFBIT", static_cast<int>(std::ios_base::eofbit));

    return result;
  }
}

namespace Rice::detail
{
  template<>
  struct Type<std::ios_base>
  {
    static bool verify()
    {
      if (!Data_Type<std::ios_base>::is_defined())
      {
        define_ios_base();
      }
      return true;
    }
  };
}
