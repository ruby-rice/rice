#ifndef Rice__Data_Type__ipp_
#define Rice__Data_Type__ipp_

#include "Class.hpp"
#include "Data_Object.hpp"
#include "detail/default_allocation_func.hpp"
#include "detail/creation_funcs.hpp"
#include "detail/method_data.hpp"
#include "detail/Caster.hpp"

#include <stdexcept>
#include <typeinfo>

template<typename T>
VALUE Rice::Data_Type<T>::klass_ = Qnil;

template<typename T>
std::auto_ptr<Rice::detail::Abstract_Caster> Rice::Data_Type<T>::caster_;

template<typename T>
template<typename Base_T>
inline Rice::Data_Type<T> Rice::Data_Type<T>::
bind(Module const & klass)
{
  if(klass.value() == klass_)
  {
    return Data_Type<T>();
  }

  if(is_bound())
  {
    std::string s;
    s = "Data type ";
    s = typeid(T).name();
    s += " is already bound to a different type";
    throw std::runtime_error(s.c_str());
  }

  // TODO: Make sure base type is bound; throw an exception otherwise.
  // We can't do this just yet, because we don't have a specialization
  // for binding to void.
  klass_ = klass;

  // TODO: do we need to unregister when the program exits?  we have to
  // be careful if we do, because the ruby interpreter might have
  // already shut down.  The correct behavior is probably to register an
  // exit proc with the interpreter, so the proc gets called before the
  // GC shuts down.
  rb_gc_register_address(&klass_);

  for(typename Instances::iterator it = unbound_instances().begin(),
      end = unbound_instances().end();
      it != end;
      unbound_instances().erase(it++))
  {
    (*it)->set_value(klass);
  }

  detail::Abstract_Caster * base_caster = Data_Type<Base_T>().caster();
  caster_.reset(new detail::Caster<T, Base_T>(base_caster, klass));
  Data_Type_Base::casters_.insert(std::make_pair(klass, caster_.get()));
  return Data_Type<T>();
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type()
  : Module_impl<Data_Type_Base, Data_Type<T> >(
      klass_ == Qnil ? rb_cObject : klass_)
{
  unbound_instances().insert(this);
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type(Module const & klass)
  : Module_impl<Data_Type_Base, Data_Type<T> >(
      klass)
{
  this->bind<void>(klass);
}

template<typename T>
inline Rice::Data_Type<T>::
~Data_Type()
{
  unbound_instances().erase(this);
}

template<typename T>
Rice::Data_Type<T> & Rice::Data_Type<T>::
operator=(Module const & klass)
{
  this->bind<void>(klass);
  return *this;
}

template<typename T>
template<typename Constructor_T>
inline Rice::Data_Type<T> & Rice::Data_Type<T>::
define_constructor(
    Constructor_T constructor)
{
  check_is_bound();

  // Normal constructor pattern with new/initialize
  detail::define_alloc_func(
      static_cast<VALUE>(*this),
      detail::default_allocation_func<T>);
  define_method("initialize", &Constructor_T::construct);

  return *this;
}

template<typename T>
inline T * Rice::Data_Type<T>::
from_ruby(Object x)
{
  check_is_bound();

  void * v = DATA_PTR(x.value());
  Class klass = x.class_of();
  Data_Type_Base::Casters::const_iterator it(
      Data_Type_Base::casters_.find(klass));
  if(it == Data_Type_Base::casters_.end())
  {
    throw std::runtime_error("Derived type is unbound");
  }

  detail::Abstract_Caster * caster = it->second;
  if(caster)
  {
    T * result = static_cast<T *>(caster->cast_to_base(v, klass_));
    return result;
  }
  else
  {
    return static_cast<T *>(v);
  }
}

template<typename T>
inline bool Rice::Data_Type<T>::
is_bound()
{
  return klass_ != Qnil;
}

template<typename T>
inline Rice::detail::Abstract_Caster * Rice::Data_Type<T>::
caster() const
{
  check_is_bound();
  return caster_.get();
}

namespace Rice
{

template<>
inline detail::Abstract_Caster * Data_Type<void>::
caster() const
{
  return 0;
}

template<typename T>
void Data_Type<T>::
check_is_bound()
{
  if(!is_bound())
  {
    std::string s;
    s = "Data type ";
    s = typeid(T).name();
    s += " is not bound";
    throw std::runtime_error(s.c_str());
  }
}

} // Rice

template<typename T>
inline Rice::Data_Type<T> Rice::
define_class_under(
    Object module,
    char const * name)
{
  Class c(define_class_under(module, name, rb_cObject));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<void>(c);
}

template<typename T, typename Base_T>
inline Rice::Data_Type<T> Rice::
define_class_under(
    Object module,
    char const * name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class_under(module, name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<Base_T>(c);
}

template<typename T>
inline Rice::Data_Type<T> Rice::
define_class(
    char const * name)
{
  Class c(define_class(name, rb_cObject));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<void>(c);
}

template<typename T, typename Base_T>
inline Rice::Data_Type<T> Rice::
define_class(
    char const * name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class(name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::template bind<Base_T>(c);
}

#endif // Rice__Data_Type__ipp_
