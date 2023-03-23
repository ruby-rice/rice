#ifndef Rice__Class__ipp_
#define Rice__Class__ipp_

#include "../Exception.hpp"
#include "../Data_Type_defn.hpp"

namespace Rice
{
  inline Class::Class(VALUE value) : Module(value)
  {
    detail::protect(rb_check_type, value, (int)T_CLASS);
  }

  inline Class& Class::undef_creation_funcs()
  {
    detail::protect(rb_undef_alloc_func, value());
    detail::protect(rb_undef_method, value(), "initialize");
    return *this;
  }

  inline Class define_class_under(Object module, char const* name, Object superclass)
  {
    return detail::protect(rb_define_class_under, module.value(), name, superclass.value());
  }

  inline Class define_class(char const* name, Object superclass)
  {
    return detail::protect(rb_define_class, name, superclass.value());
  }

  inline Class anonymous_class()
  {
    return detail::protect(rb_class_new, rb_cObject);
  }
}

namespace Rice::detail
{
  template<>
  class To_Ruby<Class>
  {
  public:
    static VALUE convert(Class const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Class>
  {
  public:
    Class convert(VALUE value)
    {
      return Class(value);
    }
  };
}
#endif // Rice__Class__ipp_