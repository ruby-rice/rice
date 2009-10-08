#ifndef Rice__Enum__ipp_
#define Rice__Enum__ipp_

#include "Data_Object.hpp"
#include "Class.hpp"
#include "String.hpp"
#include "protect.hpp"
#include <memory>

template<typename Enum_T>
long Rice::Default_Enum_Traits<Enum_T>::as_long(Enum_T value)
{
  return static_cast<long>(value);
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits>::
Enum()
  : Module_impl<Data_Type<Enum_T>, Enum<Enum_T, Enum_Traits> >()
  , enums_()
  , enums_guard_(&enums_)
  , names_()
  , names_guard_(&names_)
{
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits>::
Enum(
    char const * name,
    Module module)
  : Module_impl<Data_Type<Enum_T>, Enum<Enum_T, Enum_Traits> >()
  , enums_()
  , enums_guard_(&enums_)
  , names_()
  , names_guard_(&names_)
{
  this->template bind<void>(initialize(name, module));
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits>::
Enum(Enum<Enum_T, Enum_Traits> const & other)
  : Module_impl<Data_Type<Enum_T>, Enum<Enum_T, Enum_Traits> >(other)
  , enums_(other.enums_)
  , enums_guard_(&enums_)
  , names_(other.names_)
  , names_guard_(&names_)
{
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits> & Rice::Enum<Enum_T, Enum_Traits>::
operator=(Rice::Enum<Enum_T, Enum_Traits> const & other)
{
  Rice::Enum<Enum_T, Enum_Traits> tmp(other);
  this->swap(tmp);
  return *this;
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits>::
~Enum()
{
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits> & Rice::Enum<Enum_T, Enum_Traits>::
initialize(
    char const * name,
    Module module)
{
  Class c = Rice::define_class_under<Enum_T>(module, name)
    .define_method("to_s", to_s)
    .define_method("to_i", to_i)
    .define_method("inspect", inspect)
    .define_method("<=>", compare)
    //.define_method("hash", hash)
    .define_method("eql?", eql)
    .define_method("==", eql)
    .define_method("===", eql)
    .define_singleton_method("each", each)
    .define_singleton_method("from_int", from_int)
    .include_module(rb_mComparable);

  // TODO: This should be unnecessary (it should be taken care of when
  // define_class_under binds the C++ and ruby types)
  this->set_value(c);

  protect(rb_iv_set, c, "enums", enums_);
  protect(rb_iv_set, c, "names", names_);

  return *this;
}

template<typename Enum_T, typename Enum_Traits>
Rice::Enum<Enum_T, Enum_Traits> & Rice::Enum<Enum_T, Enum_Traits>::
define_value(
    char const * name,
    Enum_T value)
{
  std::auto_ptr<Enum_T> copy(new Enum_T(value));
  Rice::Data_Object<Enum_T> m(copy.get(), *this);
  copy.release();
  names_[m] = String(name);
  this->const_set(name, m);
  enums_.push(m);

  return *this;
}


template<typename Enum_T, typename Enum_Traits>
void Rice::Enum<Enum_T, Enum_Traits>::
swap(Enum<Enum_T, Enum_Traits> & other)
{
  Data_Type<Enum_T>::swap(other);
  enums_.swap(other.enums_);
  names_.swap(other.names_);
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
each(Object self)
{
  VALUE enums_v = rb_iv_get(self, "enums");
  Check_Type(enums_v, T_ARRAY);
  Array enums(enums_v);
  for(size_t j = 0; j < enums.size(); ++j)
  {
    rb_yield(enums[j].value());
  }
  return Qnil;
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
to_s(Object self)
{
  Data_Type<Enum_T> klass;
  Rice::Data_Object<Enum_T> m(self, klass);
  Object enum_class = rb_class_of(m);
  Hash names(rb_iv_get(enum_class, "names"));
  Object name = names[m];
  if(name.is_nil())
  {
    return String::format("INVALID(%d)", Enum_Traits::as_long(*m));
  }
  else
  {
    return String(name);
  }
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
inspect(Object self)
{
  return String::format(
      "#<%s::%s>",
      String(self.class_of().name()).c_str(),
      String(to_s(self)).c_str());
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
compare(Object lhs, Object rhs)
{
  if(lhs.class_of() != rhs.class_of())
  {
    String lhs_name(lhs.class_of().name());
    String rhs_name(rhs.class_of().name());
    rb_raise(
        rb_eTypeError,
        "Cannot compare %s to %s",
        lhs_name.c_str(),
        rhs_name.c_str());
  }

  Data_Type<Enum_T> klass;
  Rice::Data_Object<Enum_T> l(lhs, klass);
  Rice::Data_Object<Enum_T> r(rhs, klass);

  Enum_T left(*l);
  Enum_T right(*r);  

  if(left == right)
  {
    return INT2NUM(0);
  }
  else if(Enum_Traits::as_long(left) < Enum_Traits::as_long(right))
  {
    return INT2NUM(-1);
  }
  else
  {
    return INT2NUM(1);
  }
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
eql(Object lhs, Object rhs)
{
  using ::from_ruby; // Workaround for g++ 3.3.3
  bool is_equal = from_ruby<int>(compare(lhs, rhs)) == 0; 
  return to_ruby(is_equal);
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
to_i(Object self)
{
  Data_Type<Enum_T> klass;
  Rice::Data_Object<Enum_T> m(self, klass);
  return LONG2NUM(Enum_Traits::as_long(*m));
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
hash(Object self)
{
  return to_i(self);
}

template<typename Enum_T, typename Enum_Traits>
Rice::Object Rice::Enum<Enum_T, Enum_Traits>::
from_int(Class klass, Object i)
{
  using ::from_ruby; // Workaround for g++ 3.3.3
  Rice::Data_Object<Enum_T> m(
      new Enum_T(static_cast<Enum_T>(from_ruby<long>(i))),
      klass);
  return m.value();
}

template<typename T>
Rice::Enum<T> Rice::
define_enum(
    char const * name,
    Module module)
{
  return Enum<T>(name, module);
}
#endif // Rice__Enum__ipp_

