#include "detail/define_method_and_auto_wrap.hpp"
#include "Object.hpp"
#include "Data_Object.hpp"
#include "Data_Type.hpp"
#include "Symbol.hpp"
#include "protect.hpp"

#include "Module.hpp"
#include "Class.hpp"
#include "Data_Type.hpp"

#include "detail/ruby.hpp"
#include "detail/method_data.hpp"


inline
Rice::Module_base::
Module_base(VALUE v)
  : Object(v)
  , handler_(new Rice::detail::Default_Exception_Handler)
{
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
  this->handler_ =
      new Rice::detail::
      Functor_Exception_Handler<Exception_T, Functor_T>(
          functor,
          this->handler_);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename Func_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_method(
    char const * name,
    Func_T func)
{
  detail::define_method_and_auto_wrap(
      *this, name, func, this->handler_);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_singleton_method(
    char const * name,
    T func)
{
  detail::define_method_and_auto_wrap(
      rb_class_of(*this), name, func, this->handler_);
  return (Derived_T &)*this;
}

template<typename Base_T, typename Derived_T>
template<typename T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_module_function(
    char const * name,
    T func)
{
  detail::define_method_and_auto_wrap(*this, name, func);
  this->call("module_function", Symbol(name));
  return (Derived_T &)*this;
}

namespace Rice
{

namespace detail
{

class Iterator
{
public:
  virtual ~Iterator() { }

  virtual VALUE call_impl(VALUE self) = 0;

  static VALUE call(VALUE self)
  {
    void * data = Rice::detail::method_data();
    Iterator * iterator = static_cast<Iterator *>(data);
    return iterator->call_impl(self);
  }
};

template<typename T, typename Iterator_T>
class Iterator_Impl
  : public Iterator
{
public:
  Iterator_Impl(
      Iterator_T (T::*begin)(),
      Iterator_T (T::*end)(),
      Rice::Data_Type<T> data_type)
    : begin_(begin)
    , end_(end)
    , data_type_(data_type)
  {
  }

  virtual VALUE call_impl(VALUE self)
  {
    Rice::Data_Object<T> obj(self, data_type_);
    Iterator_T it = obj->begin();
    Iterator_T end = obj->end();
    for(; it != end; ++it)
    {
      protect(rb_yield, to_ruby(*it));
    }
    return self;
  }

private:
  Iterator_T (T::*begin_)();
  Iterator_T (T::*end_)();
  Rice::Data_Type<T> data_type_;
};

} // namespace detail

} // namespace Rice

template<typename Base_T, typename Derived_T>
template<typename T, typename Iterator_T>
inline
Derived_T &
Rice::Module_impl<Base_T, Derived_T>::
define_iterator(
    Iterator_T (T::*begin)(),
    Iterator_T (T::*end)(),
    char const * name)
{
  // TODO: memory leak!!!!!!!
  detail::Iterator * iterator =
    new detail::Iterator_Impl<T, Iterator_T>(
        begin,
        end,
        Data_Type<T>());
  detail::define_method_with_data(
      static_cast<VALUE>(*this),
      name,
      (RUBY_METHOD_FUNC)iterator->call,
      0,
      iterator);
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
  return result;
}

template<typename Base_T, typename Derived_T>
inline
void
Rice::Module_impl<Base_T, Derived_T>::
remove_const(Identifier name)
{
  protect(rb_mod_remove_const, *this, Symbol(name));
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
define_class(
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

