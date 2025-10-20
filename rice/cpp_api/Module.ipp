
namespace Rice
{
  inline Module::Module() : Object(rb_cObject)
  {
  }

  inline Module::Module(VALUE value) : Object(value)
  {
    if (::rb_type(value) != T_CLASS && ::rb_type(value) != T_MODULE)
    {
      throw Exception(
        rb_eTypeError,
        "Expected a Module but got a %s",
        detail::protect(rb_obj_classname, value)); // TODO: might raise an exception
    }
  }

  //! Construct a Module from an string that references a Module
  inline Module::Module(std::string name, Object under)
  {
    VALUE result = under.const_get(name);

    if (::rb_type(result) != T_MODULE)
    {
      throw Exception(
        rb_eTypeError,
        "Expected a Module but got a %s",
        detail::protect(rb_obj_classname, result)); // TODO: might raise an exception
    }

    this->set_value(result);
  }

  template<typename Function_T>
  inline void Module::wrap_native_function(VALUE klass, std::string name, Function_T&& function, MethodInfo* methodInfo)
  {
    detail::NativeFunction<Function_T>::define(klass, name, std::forward<Function_T>(function), methodInfo);
  }

  template<typename Class_T, typename Method_T>
  inline void Module::wrap_native_method(VALUE klass, std::string name, Method_T&& method, MethodInfo* methodInfo)
  {
    detail::NativeMethod<Class_T, Method_T>::define(klass, name, std::forward<Method_T>(method), methodInfo);
  }

  inline Module define_module_under(Object parent, char const* name)
  {
    VALUE module = detail::protect(rb_define_module_under, parent.value(), name);
    detail::Registries::instance.modules.add(module);
    return module;
  }

  inline Module define_module(char const* name)
  {
    VALUE module = detail::protect(rb_define_module, name);
    detail::Registries::instance.modules.add(module);
    return module;
  }

  inline Module anonymous_module()
  {
    VALUE klass = detail::protect(rb_module_new);
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
  struct Type<Module>
  {
    static bool verify()
    {
      return true;
    }

    static VALUE rubyKlass()
    {
      return rb_cModule;
    }
  };

  template<>
  class To_Ruby<Module>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Module const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };


  template<>
  class To_Ruby<Module&>
  {
  public:
    To_Ruby() = default;

    explicit To_Ruby(Return* returnInfo) : returnInfo_(returnInfo)
    {
    }

    VALUE convert(Module const& x)
    {
      return x.value();
    }

  private:
    Return* returnInfo_ = nullptr;
  };

  template<>
  class From_Ruby<Module>
  {
  public:
    Module convert(VALUE value)
    {
      return Module(value);
    }
  };
}
