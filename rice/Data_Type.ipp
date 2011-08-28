#ifndef Rice__Data_Type__ipp_
#define Rice__Data_Type__ipp_

#include "Class.hpp"
#include "String.hpp"
#include "Data_Object.hpp"
#include "detail/default_allocation_func.hpp"
#include "detail/creation_funcs.hpp"
#include "detail/method_data.hpp"
#include "detail/Caster.hpp"
#include "detail/demangle.hpp"

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

  detail::Abstract_Caster * base_caster = Data_Type<Base_T>().caster();
  caster_.reset(new detail::Caster<T, Base_T>(base_caster, klass));
  Data_Type_Base::casters().insert(std::make_pair(klass, caster_.get()));
  return Data_Type<T>();
}

template<typename T>
inline Rice::Data_Type<T>::
Data_Type()
  : Module_impl<Data_Type_Base, Data_Type<T> >(
      klass_ == Qnil ? rb_cObject : klass_)
{
  if(!is_bound())
  {
    unbound_instances().insert(this);
  }
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
  this->bind<void>(klass);
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
template<typename Constructor_T>
inline Rice::Data_Type<T> & Rice::Data_Type<T>::
define_constructor(
    Constructor_T constructor,
    Arg const& arg)
{
  Arguments* args = new Arguments();
  args->add(arg);
  return define_constructor(constructor, args);
}


template<typename T>
template<typename Director_T>
inline Rice::Data_Type<T>& Rice::Data_Type<T>::
define_director()
{
  Rice::Data_Type<Director_T>::template bind<T>(*this);
  return *this;
}

template<typename T>
inline T * Rice::Data_Type<T>::
from_ruby(Object x)
{
  check_is_bound();

  void * v = DATA_PTR(x.value());
  Class klass = x.class_of();

  if(klass.value() == klass_)
  {
    // Great, not converting to a base/derived type
    Data_Type<T> data_klass;
    Data_Object<T> obj(x, data_klass);
    return obj.get();
  }

  Data_Type_Base::Casters::const_iterator it = Data_Type_Base::casters().begin();
  Data_Type_Base::Casters::const_iterator end = Data_Type_Base::casters().end();
   
  // Finding the bound type that relates to the given klass is
  // a two step process. We iterate over the list of known type casters,
  // looking for:
  //
  // 1) casters that handle this direct type
  // 2) casters that handle types that are ancestors of klass
  //
  // Step 2 allows us to handle the case where a Rice-wrapped class
  // is subclassed in Ruby but then an instance of that class is passed
  // back into C++ (say, in a Listener / callback construction)
  //

  VALUE ancestors = rb_mod_ancestors(klass.value());

  long earliest = RARRAY_LEN(ancestors) + 1;

  int index;
  VALUE indexFound;
  Data_Type_Base::Casters::const_iterator toUse = end;

  for(; it != end; it++) {
    // Do we match directly?
    if(klass.value() == it->first) {
      toUse = it;
      break;
    }

    // Check for ancestors. Trick is, we need to find the lowest
    // ancestor that does have a Caster to make sure that we're casting
    // to the closest C++ type that the Ruby class is subclassing. 
    // There might be multiple ancestors that are also wrapped in
    // the extension, so find the earliest in the list and use that one.
    indexFound = rb_funcall(ancestors, rb_intern("index"), 1, it->first);

    if(indexFound != Qnil) {
      index = NUM2INT(indexFound);

      if(index < earliest) {
        earliest = index;
        toUse = it;
      }
    }
  }
  
  if(toUse == end)
  {
    std::string s = "Class ";
    s += klass.name().str();
    s += " is not registered/bound in Rice";
    throw std::runtime_error(s);
  }

  detail::Abstract_Caster * caster = toUse->second;
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

template<typename From_T, typename To_T>
inline void 
Rice::define_implicit_cast()
{
  // As Rice currently expects only one entry into
  // this list for a given klass VALUE, we need to get
  // the current caster for From_T and insert in our
  // new caster as the head of the caster list

  Class from_class = Data_Type<From_T>::klass().value();
  Class to_class = Data_Type<To_T>::klass().value();

  detail::Abstract_Caster* from_caster = 
    Data_Type<From_T>::caster_.release();

  detail::Abstract_Caster* new_caster = 
    new detail::Implicit_Caster<To_T, From_T>(from_caster, to_class);

  // Insert our new caster into the list for the from class
  Data_Type_Base::casters().erase(from_class);
  Data_Type_Base::casters().insert(
    std::make_pair(
      from_class,
      new_caster
    )
  );

  // And make sure the from_class has direct access to the
  // updated caster list
  Data_Type<From_T>::caster_.reset(new_caster);
}

#endif // Rice__Data_Type__ipp_
