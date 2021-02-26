#ifndef Rice__Data_Type__ipp_
#define Rice__Data_Type__ipp_

#include "detail/default_allocation_func.hpp"
#include "detail/method_data.hpp"
#include "detail/Caster.hpp"
#include "detail/Wrapper.hpp"
#include "detail/demangle.hpp"
#include "detail/Iterator.hpp"
#include "Class.hpp"
#include "String.hpp"

#include <stdexcept>
#include <typeinfo>

namespace Rice
{

template<typename T>
void ruby_mark_internal(detail::Wrapper<T>* wrapper)
{
  // Tell the wrapper to mark the objects its keeping alive
  wrapper->ruby_mark();

  // Get the underlying data and call custom mark function (if any)
  T* data = wrapper->get();
  ruby_mark<T>(data);
}

template<typename T>
void ruby_free_internal(detail::Wrapper<T>* wrapper)
{
  delete wrapper;
}

template<typename T>
size_t ruby_size_internal(const T* data)
{
  return sizeof(T);
}

template<typename T>
inline Data_Type<T> Data_Type<T>::
bind(Module const& klass)
{
  if (klass.value() == klass_)
  {
    return Data_Type<T>();
  }

  if (is_bound())
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

  rb_type_ = new rb_data_type_t();
  rb_type_->wrap_struct_name = strdup(rb_class2name(klass));
  rb_type_->function.dmark = reinterpret_cast<void(*)(void*)>(&Rice::ruby_mark_internal<T>);
  rb_type_->function.dfree = reinterpret_cast<void(*)(void*)>(&Rice::ruby_free_internal<T>);
  rb_type_->function.dsize = reinterpret_cast<size_t(*)(const void*)>(&Rice::ruby_size_internal<T>);
  rb_type_->data = nullptr;
  rb_type_->flags = RUBY_TYPED_FREE_IMMEDIATELY;

  for (typename Instances::iterator it = unbound_instances().begin(),
    end = unbound_instances().end();
    it != end;
    unbound_instances().erase(it++))
  {
    (*it)->set_value(klass);
  }

  return Data_Type<T>();
}

template<typename T>
inline Data_Type<T>::
Data_Type()
  : Class(
    klass_ == Qnil ? rb_cObject : klass_)
{
  if (!is_bound())
  {
    unbound_instances().insert(this);
  }
}

template<typename T>
inline Data_Type<T>::
  Data_Type(Module const& klass)
  : Class(klass)
{
  this->bind(klass);
}

template<typename T>
inline Data_Type<T>::
~Data_Type()
{
  unbound_instances().erase(this);
}

template<typename T>
rb_data_type_t* Data_Type<T>::rb_type()
{
  check_is_bound();
  return rb_type_;
}

template<typename T>
Module Data_Type<T>::klass()
{
  check_is_bound();
  return klass_;
}

template<typename T>
Data_Type<T>& Data_Type<T>::
  operator=(Module const& klass)
{
  this->bind(klass);
  return *this;
}

template<typename T>
template<typename Constructor_T>
inline Data_Type<T>& Data_Type<T>::
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
inline Data_Type<T>& Data_Type<T>::
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
inline Data_Type<T>& Data_Type<T>::
define_director()
{
  Data_Type<Director_T>::bind(*this);

  // TODO - hack to fake Ruby into thinking that a Director is
  // the same as the base data type
  Data_Type<Director_T>::rb_type_ = Data_Type<T>::rb_type_;
  return *this;
}

template<typename T>
inline bool Data_Type<T>::
is_bound()
{
  return klass_ != Qnil;
}

template<typename T>
bool Data_Type<T>::
is_descendant(VALUE value)
{
  check_is_bound();
  return rb_obj_is_kind_of(value, klass_) == Qtrue;
}

template<typename T>
void Data_Type<T>::
check_is_bound()
{
  if (!is_bound())
  {
    std::string s;
    s = "Data type ";
    s += detail::demangle(typeid(T).name());
    s += " is not bound";
    throw std::runtime_error(s.c_str());
  }
}

template<typename T>
inline Data_Type<T> 
define_class_under(
    Object module,
    char const* name)
{
  Class c(define_class_under(module, name, rb_cObject));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
}

template<typename T, typename Base_T>
inline Data_Type<T> 
  define_class_under(
    Object module,
    char const* name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class_under(module, name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
}

template<typename T>
inline Data_Type<T> 
define_class(
    char const* name)
{
  Class c(define_class(name, rb_cObject));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
}

template<typename T, typename Base_T>
inline Data_Type<T> 
define_class(
    char const* name)
{
  Data_Type<Base_T> base_dt;
  Class c(define_class(name, base_dt));
  c.undef_creation_funcs();
  return Data_Type<T>::bind(c);
}

template<typename From_T, typename To_T>
void define_implicit_cast()
{
  Class from_class = Data_Type<From_T>::klass().value();
  Class to_class = Data_Type<To_T>::klass().value();
  detail::CasterRegistry::add<From_T, To_T>(from_class, to_class);
}

template<typename T>
template<typename U, typename Iterator_T>
inline Data_Type<T>& Data_Type<T>::
  define_iterator(Iterator_T(U::* begin)(), Iterator_T(U::* end)(), Identifier name)
{
  using Iter_T = detail::Iterator<U, Iterator_T>;
  Iter_T* iterator = new Iter_T(begin, end);
  detail::MethodData::define_method(Data_Type<T>::klass(), name,
    (RUBY_METHOD_FUNC)iterator->call, 0, iterator);

  return *this;
}

} //namespace

#endif