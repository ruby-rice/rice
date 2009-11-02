#include "Object.hpp"
#include "Class.hpp"
#include "String.hpp"
#include "Array.hpp"

#include <algorithm>
#include <iostream>

namespace Rice
{

Object const Nil(Qnil);
Object const True(Qtrue);
Object const False(Qfalse);
Object const Undef(Qundef);

}

Rice::Class Rice::Object::
class_of() const {
  return rb_class_of(value_);
}

int Rice::Object::
compare(Object const & other) const
{
  Object result = call("<=>", other);
  return from_ruby<int>(result);
}

void Rice::Object::
freeze()
{
  protect(rb_obj_freeze, value());
}

bool Rice::Object::
is_frozen() const
{
  return bool(OBJ_FROZEN(value()));
}

Rice::String Rice::Object::
to_s() const {
  return call("to_s");
}

Rice::String Rice::Object::
inspect() const {
  return call("inspect");
}

void Rice::Object::
swap(Rice::Object & other)
{
  std::swap(value_, other.value_);
}

Rice::Object Rice::Object::
instance_eval(String const & s)
{
  VALUE argv[] = { s.value() };
  return protect(rb_obj_instance_eval, 1, &argv[0], *this);
}

int Rice::Object::
rb_type() const
{
  return ::rb_type(*this);
}

bool Rice::Object::
is_a(Object klass) const
{
  Object result = protect(rb_obj_is_kind_of, *this, klass);
  return result.test();
}

bool Rice::Object::
respond_to(Identifier id) const
{
  return bool(rb_respond_to(*this, id));
}

bool Rice::Object::
is_instance_of(Object klass) const
{
  Object result = protect(rb_obj_is_instance_of, *this, klass);
  return result.test();
}

Rice::Object Rice::Object::
iv_get(
    Identifier name) const
{
  return protect(rb_ivar_get, *this, name.id());
}

Rice::Object Rice::Object::
attr_get(
    Identifier name) const
{
  return protect(rb_attr_get, *this, name.id());
}

Rice::Object Rice::Object::
vcall(
    Identifier id,
    Array args)
{
  return protect(rb_funcall3, *this, id, args.size(), args.to_c_array());
}

void Rice::Object::
mark() const
{
  rb_gc_mark(*this);
}

void Rice::Object::
set_value(VALUE v)
{
  value_ = v;
}

std::ostream & Rice::
operator<<(std::ostream & out, Rice::Object const & obj)
{
  String s(obj.to_s());
  out << s.c_str();
  return out;
}

bool Rice::
operator==(Rice::Object const & lhs, Rice::Object const & rhs)
{
  Object result = lhs.call("==", rhs);
  return result.test();
}

bool Rice::
operator!=(Rice::Object const & lhs, Rice::Object const & rhs)
{
  return !(lhs == rhs);
}

bool Rice::
operator<(Rice::Object const & lhs, Rice::Object const & rhs)
{
  Object result = lhs.call("<", rhs);
  return result.test();
}

bool Rice::
operator>(Rice::Object const & lhs, Rice::Object const & rhs)
{
  Object result = lhs.call(">", rhs);
  return result.test();
}

