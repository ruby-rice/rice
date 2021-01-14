#ifndef Rice__Module_impl__ipp_
#define Rice__Module_impl__ipp_

#include "detail/define_method_and_auto_wrap.hpp"

inline
Rice::Module_base::
Module_base(VALUE v)
  : Object(v), handler_(std::make_shared<Rice::detail::Default_Exception_Handler>())
{
}

inline
Rice::Module_base::
Module_base(Module_base const & other)
  : Object(other)
  , handler_(other.handler_)
{
}

inline
Rice::Module_base &
Rice::Module_base::
operator=(Module_base const & other)
{
  Module_base tmp(other);
  swap(tmp);
  return *this;
}

inline
void
Rice::Module_base::
swap(Module_base & other)
{
  std::swap(handler_, other.handler_);
  Object::swap(other);
}

template<typename Exception_T, typename Functor_T>
inline
void
Rice::Module_base::
add_handler(Functor_T functor)
{
  // Create a new exception handler and pass ownership of the current handler to it (they
  // get chained together). Then take ownership of the new handler.
  this->handler_ = std::make_shared<detail::Functor_Exception_Handler<Exception_T, Functor_T>>(
    functor, std::move(this->handler_));
}

inline
std::shared_ptr<Rice::detail::Exception_Handler>
Rice::Module_base::
handler() const
{
  return this->handler_;
}

inline
Rice::Module_impl::
Module_impl()
  : Module_base()
{
}

template<typename T>
inline
Rice::Module_impl::
Module_impl(T const & arg)
  : Module_base(arg)
{
}

template<typename Exception_T, typename Functor_T>
inline
Rice::Module_impl&
Rice::Module_impl::
add_handler(
    Functor_T functor)
{
  Module_base::add_handler<Exception_T>(functor);
  return *this;
}

template<typename Func_T>
inline
Rice::Module_impl&
Rice::Module_impl::
define_method(
    Identifier name,
    Func_T func,
    Arguments* arguments)
{
  detail::define_method_and_auto_wrap(
      *this, name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module_impl&
Rice::Module_impl::
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
Rice::Module_impl&
Rice::Module_impl::
define_singleton_method(
    Identifier name,
    Func_T func,
    Arguments* arguments)
{
  detail::define_method_and_auto_wrap(
      rb_singleton_class(*this), name, func, this->handler(), arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module_impl&
Rice::Module_impl::
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
Rice::Module_impl&
Rice::Module_impl::
define_module_function(
    Identifier name,
    Func_T func,
    Arguments* arguments)
{
  if(this->rb_type() != T_MODULE)
  {
    throw std::runtime_error("can only define module functions for modules");
  }

  define_method(name, func, arguments);
  define_singleton_method(name, func, arguments);
  return *this;
}

template<typename Func_T>
inline
Rice::Module_impl&
Rice::Module_impl::
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
Rice::Module_impl&
Rice::Module_impl::
const_set(
    Identifier name,
    Object value)
{
  protect(detail::const_set, *this, name, value);
  return *this;
}

inline
Rice::Object
Rice::Module_impl::
const_get(
    Identifier name) const
{
  return protect(rb_const_get, *this, name);
}

inline
bool
Rice::Module_impl::
const_defined(Identifier name) const
{
  size_t result = protect(rb_const_defined, *this, name);
  return bool(result);
}

inline
void
Rice::Module_impl::
remove_const(Identifier name)
{
  protect(rb_mod_remove_const, *this, name.to_sym());
}

template<typename T>
Rice::Data_Type<T>
Rice::Module_impl::
define_class(
  char const* name)
{
  return this->define_class_with_object_as_base<T>(name);
}

template<typename T>
inline
Rice::Data_Type<T>
Rice::Module_impl::
define_class_with_object_as_base(
    char const * name)
{
  return Rice::define_class_under<T>(*this, name);
}

template<typename T, typename T_Base_T>
inline
Rice::Data_Type<T>
Rice::Module_impl::
define_class(
    char const * name)
{
  return Rice::define_class_under<T, T_Base_T>(
      *this,
      name);
}

#endif 