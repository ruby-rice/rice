#ifndef Rice__Module__ipp_
#define Rice__Module__ipp_

#include "../detail/rice_traits.hpp"
#include "../detail/function_traits.hpp"
#include "../detail/Type.hpp"
#include "../detail/NativeFunction.hpp"
#include "../Exception.hpp"

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

  template<typename Exception_T, typename Functor_T>
  inline Module& Module::add_handler(Functor_T functor)
  {
    // Create a new exception handler and pass ownership of the current handler to it (they
    // get chained together). Then take ownership of the new handler.
    this->handler_ = std::make_shared<detail::Functor_Exception_Handler<Exception_T, Functor_T>>(
      functor, std::move(this->handler_));

    return *this;
  }

  inline std::shared_ptr<detail::Exception_Handler> Module::handler() const
  {
    return this->handler_;
  }

  template<bool IsMethod, typename Function_T>
  inline void Module::wrap_native_call(VALUE klass, Identifier name, Function_T&& function,
    std::shared_ptr<detail::Exception_Handler> handler, MethodInfo* methodInfo)
  {
    auto* native = new detail::NativeFunction<VALUE, Function_T, IsMethod>(std::forward<Function_T>(function), handler, methodInfo);
    using Native_T = typename std::remove_pointer_t<decltype(native)>;

    detail::verifyType<typename Native_T::Return_T>();
    detail::verifyTypes<typename Native_T::Arg_Ts>();

    detail::MethodData::define_method(klass, name.id(), &Native_T::call, -1, native);
  }

  inline Object Module::const_get(Identifier name) const
  {
    return detail::protect(rb_const_get, this->value(), name.id());
  }

  inline bool Module::const_defined(Identifier name) const
  {
    size_t result = detail::protect(rb_const_defined, this->value(), name.id());
    return bool(result);
  }

  inline void Module::remove_const(Identifier name)
  {
    detail::protect(rb_mod_remove_const, this->value(), name.to_sym());
  }

  inline Module define_module_under(Object module, char const* name)
  {
    return detail::protect(rb_define_module_under, module.value(), name);
  }

  inline Module define_module(char const* name)
  {
    return detail::protect(rb_define_module, name);
  }

  inline Module anonymous_module()
  {
    return detail::protect(rb_module_new);
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
  };

  template<>
  class To_Ruby<Module>
  {
  public:
    VALUE convert(Module const& x)
    {
      return x.value();
    }
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
#endif // Rice__Module__ipp_