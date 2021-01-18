#ifndef Rice__Module__ipp_
#define Rice__Module__ipp_

#include "detail/wrap_function.hpp"
#include "Exception.hpp"
#include "protect.hpp"

inline
Rice::Module::
Module()
  : Object(rb_cObject)
{
}

inline
Rice::Module::
Module(VALUE v)
  : Object(v)
{
  if (::rb_type(v) != T_CLASS && ::rb_type(v) != T_MODULE)
  {
    throw Exception(
      rb_eTypeError,
      "Expected a Module but got a %s",
      rb_class2name(CLASS_OF(v))); // TODO: might raise an exception
  }
}

inline
Rice::Module Rice::
define_module_under(
  Rice::Object module,
  char const* name)
{
  VALUE v = rb_define_module_under(module, name);
  return Module(v);
}

inline
Rice::Module Rice::
define_module(
  char const* name)
{
  VALUE v = rb_define_module(name);
  return Module(v);
}

inline
Rice::Module Rice::
anonymous_module()
{
  return Module(protect(rb_module_new));
}

template<typename Exception_T, typename Functor_T>
inline
Rice::Module&
Rice::Module::
add_handler(
  Functor_T functor)
{
  // Create a new exception handler and pass ownership of the current handler to it (they
  // get chained together). Then take ownership of the new handler.
  this->handler_ = std::make_shared<detail::Functor_Exception_Handler<Exception_T, Functor_T>>(
    functor, std::move(this->handler_));

  return *this;
}

inline
std::shared_ptr<Rice::detail::Exception_Handler>
Rice::Module::
handler() const
{
  return this->handler_;
}

inline
Rice::Module&
Rice::Module::
include_module(Rice::Module const& inc)
{
  Rice::protect(rb_include_module, *this, inc);
  return *this;
}

template<typename Fun_T>
inline void Rice::Module::define_method_and_auto_wrap(VALUE klass, Identifier name, Fun_T function,
                                                      std::shared_ptr<detail::Exception_Handler> handler,
                                                      Arguments* arguments)
{
  auto* wrapper = detail::wrap_function(function, handler, arguments);
  using WrapperType = std::remove_pointer_t<decltype(wrapper)>;

  Data_Object<WrapperType> f(wrapper, rb_cObject);

  Rice::protect(
    detail::define_method_with_data,
    klass,
    name.id(),
    RUBY_METHOD_FUNC(&WrapperType::call),
    -1,
    f);
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_method(
  Identifier name,
  Func_T func,
  Arguments* arguments)
{
  this->define_method_and_auto_wrap(*this, name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_method(
  Identifier name,
  Func_T func,
  Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_method(name, func, args);
}


template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_singleton_method(
  Identifier name,
  Func_T func,
  Arguments* arguments)
{
  this->define_method_and_auto_wrap(rb_singleton_class(*this), name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_singleton_method(
  Identifier name,
  Func_T func,
  Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_singleton_method(name, func, args);
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_module_function(
  Identifier name,
  Func_T func,
  Arguments* arguments)
{
  if (this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_method(name, func, arguments);
  define_singleton_method(name, func, arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module&
Rice::Module::
define_module_function(
  Identifier name,
  Func_T func,
  Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_module_function(name, func, args);
}

namespace Rice
{

  namespace detail
  {


    inline VALUE
      const_set(VALUE mod, ID name, VALUE value)
    {
      rb_const_set(mod, name, value);
      return Qnil;
    }

  } // namespace detail

} // namespace Rice

inline
Rice::Module&
Rice::Module::
const_set(
  Identifier name,
  Object value)
{
  protect(detail::const_set, *this, name, value);
  return *this;
}

inline
Rice::Object
Rice::Module::
const_get(
  Identifier name) const
{
  return protect(rb_const_get, *this, name);
}

inline
bool
Rice::Module::
const_defined(Identifier name) const
{
  size_t result = protect(rb_const_defined, *this, name);
  return bool(result);
}

inline
void
Rice::Module::
remove_const(Identifier name)
{
  protect(rb_mod_remove_const, *this, name.to_sym());
}

template<typename T>
Rice::Data_Type<T>
Rice::Module::
define_class(
  char const* name)
{
  return this->define_class_with_object_as_base<T>(name);
}

template<typename T>
inline
Rice::Data_Type<T>
Rice::Module::
define_class_with_object_as_base(
  char const* name)
{
  return Rice::define_class_under<T>(*this, name);
}

template<typename T, typename T_Base_T>
inline
Rice::Data_Type<T>
Rice::Module::
define_class(
  char const* name)
{
  return Rice::define_class_under<T, T_Base_T>(
    *this,
    name);
}


#endif // Rice__Module__ipp_
