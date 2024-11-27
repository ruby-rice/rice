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

  template<typename ...Arg_Ts>
  inline Object Class::create(Arg_Ts ...args)
  {
    return this->call("new", args...);
  }

  inline const std::string Class::name()
  {
    const char* buffer = rb_class2name(this->value());
    return std::string(buffer);
  }

  inline Class define_class_under(Object module, char const* name, const Class& superclass)
  {
    VALUE klass = detail::protect(rb_define_class_under, module.value(), name, superclass.value());

    // We MUST reset the instance registry in case the user just redefined a class which resets it
    detail::Registries::instance.natives.reset(klass);

    return klass;
  }

  inline Class define_class(char const* name, const Class& superclass)
  {
    VALUE klass = detail::protect(rb_define_class, name, superclass.value());

    // We MUST reset the instance registry in case the user just redefined a class which resets it
    detail::Registries::instance.natives.reset(klass);

    return klass;
  }

  inline Class anonymous_class()
  {
    VALUE klass = detail::protect(rb_class_new, rb_cObject);
    VALUE singleton = detail::protect(rb_singleton_class, klass);

    // Ruby will reuse addresses previously assigned to other modules
    // that have subsequently been garbage collected
    detail::Registries::instance.natives.reset(klass);
    detail::Registries::instance.natives.reset(singleton);

    return klass;
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