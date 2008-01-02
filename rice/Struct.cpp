#include "Struct.hpp"
#include "Symbol.hpp"

#include <stdexcept>

Rice::Struct::
Struct()
  : Module_impl<Class, Struct>(rb_cObject)
  , members_()
  , members_guard_(&members_)
  , member_offset_()
  , member_offset_guard_(&member_offset_)
{
}

Rice::Struct::
Struct(Rice::Struct const & s)
  : Module_impl<Class, Struct>(s)
  , members_(s.members_.value())
  , members_guard_(&members_)
  , member_offset_(s.member_offset_.value())
  , member_offset_guard_(&member_offset_)
{
}

Rice::Struct::
~Struct()
{
}

Rice::Struct & Rice::Struct::
initialize(
    Module module,
    Identifier name)
{
  Class struct_class(rb_cStruct);
  Object type = struct_class.vcall("new", members_);

  set_value(type);
  module.const_set(name, type);

  return *this;
}

Rice::Struct & Rice::Struct::
define_member(
    Identifier name)
{
  if(value() != rb_cObject)
  {
    throw std::runtime_error("struct is already initialized");
  }

  // TODO: not exception-safe
  Symbol ruby_name(name);
  member_offset_[ruby_name] = members_.size();
  members_.push(ruby_name);

  return *this;
}

size_t Rice::Struct::
offset_of(Identifier name) const
{
  Symbol ruby_name(name);
  return from_ruby<size_t>(member_offset_[ruby_name]);
}

void Rice::Struct::
swap(Rice::Struct & other)
{
  members_.swap(other.members_);
  members_guard_.swap(other.members_guard_);
  member_offset_.swap(other.member_offset_);
  member_offset_guard_.swap(other.member_offset_guard_);
  Class::swap(other);
}

Rice::Struct::Instance
Rice::Struct::
new_instance(Array args) const
{
  Object instance = const_cast<Struct *>(this)->vcall("new", args);
  return Instance(*this, instance);
}

Rice::Struct::Instance::
Instance(
    Struct const & type,
    Array args)
  : Builtin_Object<RStruct, T_STRUCT>(type.new_instance(args))
  , type_(type)
{
}

Rice::Struct::Instance::
Instance(
    Struct const & type,
    Object s)
  : Builtin_Object<RStruct, T_STRUCT>(s)
  , type_(type)
{
}

void Rice::Struct::Instance::
swap(Instance & other)
{
  type_.swap(other.type_);
  Builtin_Object<RStruct, T_STRUCT>::swap(other);
}

Rice::Struct Rice::
define_struct()
{
  return Struct();
}

