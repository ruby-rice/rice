#ifndef Rice__Data_Type__ipp_
#define Rice__Data_Type__ipp_

#include "detail/default_allocation_func.hpp"
#include "detail/method_data.hpp"
#include "detail/Caster.hpp"
#include "detail/demangle.hpp"
#include "detail/Iterator.hpp"

#include "Class.hpp"
#include "String.hpp"

#include <stdexcept>
#include <typeinfo>

template<typename T>
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
    s += detail::demangle(typeid(T).name());
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

  return Data_Type<T>();
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type()
  : Class(
      klass_ == Qnil ? rb_cData : klass_)
{
  if(!is_bound())
  {
    unbound_instances().insert(this);
  }
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type(Module const & klass)
  : Class(klass)
{
  this->bind(klass);
}

template<typename T>
inline Rice::Data_Type<T>::
~Data_Type()
{
  unbound_instances().erase(this);
}

template<typename T>
Rice::Module
Rice::Data_Type<T>::
klass() {
  if(is_bound())
  {
    return klass_;
  }
  else
  {
    std::string s;
    s += detail::demangle(typeid(T *).name());
    s += " is unbound";
    throw std::runtime_error(s.c_str());
  }
}

template<typename T>
Rice::Data_Type<T> & Rice::Data_Type<T>::
operator=(Module const & klass)
{
  this->bind(klass);
  return *this;
}

template<typename T>
template<typename Constructor_T>
inline Rice::Data_Type<T> & Rice::Data_Type<T>::
define_constructor(
    Constructor_T /* constructor */,
    Arguments* arguments)
{
  check_is_bound();

  // Normal constructor pattern with new/initialize
  rb_define_alloc_func(
      static_cast<VALUE>(*this),
      detail::default_allocation_func<T>);
  this->define_method(
      "initialize",
      &Constructor_T::construct,
      arguments
      );

  return *this;
}

template<typename T>
template<typename Constructor_T, typename...Arg_Ts>
inline Rice::Data_Type<T> & Rice::Data_Type<T>::
define_constructor(
    Constructor_T constructor,
    Arg_Ts const& ...args)
{
  check_is_bound();

  // Define a Ruby allocator which creates the Ruby object
  rb_define_alloc_func(static_cast<VALUE>(*this), detail::default_allocation_func<T>);

  // Define an initialize function that will create the C++ object
  this->define_method("initialize", &Constructor_T::construct, args...);

  return *this;
}

template<typename T>
template<typename Director_T>
inline Rice::Data_Type<T>& Rice::Data_Type<T>::
define_director()
{
  Rice::Data_Type<Director_T>::bind(*this);
  return *this;
}

template<typename T>
inline T * Rice::Data_Type<T>::
from_ruby(Object x)
{
  check_is_bound();

  void * v = DATA_PTR(x.value());
  VALUE klass = rb_class_of(x);

  if (klass == klass_)
  {
    // Great, not converting to a base/derived type
    Data_Type<T> data_klass;
    Data_Object<T> obj(x, data_klass);
    return obj.get();
  }
  else if (rb_class_inherited_p(klass, klass_) == Qtrue)
  {
    // The native function is asking for an ancestor class so we are good to go...
    return static_cast<T*>(v);
  }
  else if (detail::AbstractCaster* caster = detail::AbstractCaster::find(klass, klass_); caster)
  {
    // An implicit type conversion has been registered so use it
    return static_cast<T*>(caster->cast(v));
  }
  else
  {
      std::string s = "Bad cast. No caster found for ";
      s += Class(klass).name().str();
      throw std::runtime_error(s);
  }
}

template<typename T>
inline bool Rice::Data_Type<T>::
is_bound()
{
  return klass_ != Qnil;
}

namespace Rice
{

template<typename T>
void Data_Type<T>::
check_is_bound()
{
  if(!is_bound())
  {
    std::string s;
    s = "Data type ";
    s += detail::demangle(typeid(T).name());
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
  Class c(define_class_under(module, name, rb_cData));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
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
  return Data_Type<T>::bind(c);
}

template<typename T>
inline Rice::Data_Type<T> Rice::
define_class(
    char const * name)
{
  Class c(define_class(name, rb_cData));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
}

template<typename T, typename Base_T>
inline Rice::Data_Type<T> Rice::
define_class(
    char const * name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class(name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
}

template<typename T>
template<typename U, typename Iterator_T>
inline Rice::Data_Type<T>& Rice::Data_Type<T>::
define_iterator(Iterator_T(U::* begin)(), Iterator_T(U::* end)(), Identifier name)
{
  using Iter_T = detail::Iterator<U, Iterator_T>;
  Iter_T* iterator = new Iter_T(begin, end);
  detail::MethodData::define_method(Data_Type<T>::klass(), name, 
    (RUBY_METHOD_FUNC)iterator->call, 0, iterator);

  return *this;
}

template<typename From_T, typename To_T>
inline void 
Rice::define_implicit_cast()
{
  Class from_class = Data_Type<From_T>::klass().value();
  Class to_class = Data_Type<To_T>::klass().value();
  detail::AbstractCaster* caster = new detail::Caster<From_T, To_T>();
  detail::AbstractCaster::registerCaster(from_class, to_class, caster);
}

#endif // Rice__Data_Type__ipp_
