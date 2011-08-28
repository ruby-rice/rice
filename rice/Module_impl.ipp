#include "detail/define_method_and_auto_wrap.hpp"
#include "Object.hpp"
#include "Address_Registration_Guard.hpp"
#include "Data_Object.hpp"
#include "Data_Type.hpp"
#include "Symbol.hpp"
#include "Exception.hpp"
#include "protect.hpp"

#include "Module.hpp"
#include "Class.hpp"
#include "Data_Type.hpp"

#include "detail/ruby.hpp"
#include "detail/method_data.hpp"
#include "detail/Iterator.hpp"


inline
Rice::Module_base::
Module_base(VALUE v)
  : Object(v)
  , handler_(Qnil)
  , handler_guard_(&handler_)
{
}

inline
Rice::Module_base::
Module_base(Module_base const & other)
  : Object(other)
  , handler_(other.handler_)
  , handler_guard_(&handler_)
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
  Data_Object<detail::Exception_Handler> handler(
      new detail::
      Functor_Exception_Handler<Exception_T, Functor_T>(
          functor,
          this->handler()),
      rb_cObject);
  this->handler_.swap(handler);
}

inline
Rice::Object
Rice::Module_base::
handler() const
{
  if(!handler_.test())
  {
    Data_Object<Rice::detail::Default_Exception_Handler> handler(
        new Rice::detail::Default_Exception_Handler,
        rb_cObject);
    handler_.swap(handler);
  }

  return handler_;
}

template<typename Base_T, typename Derived_T>
inline
Rice::Module_impl<Base_T, Derived_T>::
Module_impl()
  : Base_T()
{
}

template<typename Base_T, typename Derived_T>
template<typename T>
inline
Rice::Module_impl<Base_T, Derived_T>::
Module_impl(T const & arg)
  : Base_T(arg)
{
}

template<typename Base_T, typename Derived_T>
template<typename Exception_T, typename Functor_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
add_handler(
    Functor_T functor)
{
  Module_base::add_handler<Exception_T>(functor);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_method(
    Identifier name,
    Func_T func,
    Arguments* arguments)
{
  detail::define_method_and_auto_wrap(
      *this, name, func, this->handler(), arguments);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_method(
    Identifier name,
    Func_T func,
    Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_method(name, func, args);
}


template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_singleton_method(
    Identifier name,
    Func_T func,
    Arguments* arguments)
{
  detail::define_method_and_auto_wrap(
      rb_class_of(*this), name, func, this->handler(), arguments);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_singleton_method(
    Identifier name,
    Func_T func,
    Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_singleton_method(name, func, args);
}

template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_module_function(
    Identifier name,
    Func_T func,
    Arguments* arguments)
{
  if(this->rb_type() != T_MODULE)
  {
    throw Rice::Exception(
        rb_eTypeError,
        "can only define module functions for modules");
  }

  define_method(name, func, arguments);
  define_singleton_method(name, func, arguments);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_module_function(
    Identifier name,
    Func_T func,
    Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_module_function(name, func, args);
}

template<typename Base_T, typename Derived_T>
template<typename T, typename Iterator_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_iterator(
    Iterator_T (T::*begin)(),
    Iterator_T (T::*end)(),
    Identifier name)
{
  detail::define_iterator(*this, name, begin, end);
  return (Derived_T &)*this;
}

namespace Rice
{

namespace detail
{

inline VALUE
  include_module(VALUE mod, VALUE inc)
{
  rb_include_module(mod, inc);
  return Qnil;
}

inline VALUE
const_set(VALUE mod, ID name, VALUE value)
{
  rb_const_set(mod, name, value);
  return Qnil;
}

} // namespace detail

} // namespace Rice

template<typename Base_T, typename Derived_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
include_module(
    Module const & inc)
{
  protect(detail::include_module, *this, inc);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
const_set(
    Identifier name,
    Object value)
{
  protect(detail::const_set, *this, name, value);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
inline
Rice::Object
Rice::Module_impl<Base_T, Derived_T>::
const_get(
    Identifier name) const
{
  return protect(rb_const_get, *this, name);
}

template<typename Base_T, typename Derived_T>
inline
bool
Rice::Module_impl<Base_T, Derived_T>::
const_defined(Identifier name) const
{
  int result = protect(rb_const_defined, *this, name);
  return bool(result);
}

template<typename Base_T, typename Derived_T>
inline
void
Rice::Module_impl<Base_T, Derived_T>::
remove_const(Identifier name)
{
  protect(rb_mod_remove_const, *this, name.to_sym());
}

template<typename Base_T, typename Derived_T>
inline
Rice::Module
Rice::Module_impl<Base_T, Derived_T>::
define_module(
    char const * name)
{
  return Rice::define_module_under(*this, name);
}

template<typename Base_T, typename Derived_T>
inline
Rice::Class
Rice::Module_impl<Base_T, Derived_T>::
define_class(
    char const * name,
    Object superclass)
{
  return Rice::define_class_under(*this, name, superclass);
}

template<typename Base_T, typename Derived_T>
template<typename T>
inline
Rice::Data_Type<T>
Rice::Module_impl<Base_T, Derived_T>::
define_class_with_object_as_base(
    char const * name)
{
  return Rice::define_class_under<T>(*this, name);
}

template<typename Base_T, typename Derived_T>
template<typename T, typename T_Base_T>
inline
Rice::Data_Type<T>
Rice::Module_impl<Base_T, Derived_T>::
define_class(
    char const * name)
{
  return Rice::define_class_under<T, T_Base_T>(
      *this,
      name);
}

