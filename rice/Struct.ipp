#include "Symbol.hpp"

inline Rice::Struct::
Struct()
  : Class(rb_cObject)
  , members_()
  , members_guard_(&members_)
  , member_offset_()
  , member_offset_guard_(&member_offset_)
{
}

inline Rice::Struct::
Struct(Rice::Struct const& s)
  : Class(s)
  , members_(s.members_.value())
  , members_guard_(&members_)
  , member_offset_(s.member_offset_.value())
  , member_offset_guard_(&member_offset_)
{
}

inline Rice::Struct::
~Struct()
{
}

inline Rice::Struct& Rice::Struct::
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

inline Rice::Struct& Rice::Struct::
define_member(
    Identifier name)
{
  if (value() != rb_cObject)
  {
    throw std::runtime_error("struct is already initialized");
  }

  // TODO: not exception-safe
  Symbol ruby_name(name);
  member_offset_[ruby_name] = members_.size();
  members_.push(ruby_name);

  return *this;
}

inline unsigned long Rice::Struct::
offset_of(Identifier name) const
{
  Symbol ruby_name(name);
  return Rice::detail::From_Ruby<unsigned long>::convert(member_offset_[ruby_name].value());
}

inline void Rice::Struct::
swap(Rice::Struct& other)
{
  members_.swap(other.members_);
  members_guard_.swap(other.members_guard_);
  member_offset_.swap(other.member_offset_);
  member_offset_guard_.swap(other.member_offset_guard_);
  Class::swap(other);
}

inline Rice::Struct::Instance
Rice::Struct::
  new_instance(Array args) const
{
  Object instance = const_cast<Struct*>(this)->vcall("new", args);
  return Instance(*this, instance);
}

inline Rice::Struct::Instance::
Instance(
    Struct const& type,
    Array args)
  : Builtin_Object<T_STRUCT>(type.new_instance(args))
  , type_(type)
{
}

inline Rice::Struct::Instance::
Instance(
    Struct const& type,
    Object s)
  : Builtin_Object<T_STRUCT>(s)
  , type_(type)
{
}

inline void Rice::Struct::Instance::
swap(Instance& other)
{
  type_.swap(other.type_);
  Builtin_Object<T_STRUCT>::swap(other);
}

inline Rice::Struct Rice::
define_struct()
{
  return Struct();
}


template<typename T>
inline Rice::Object Rice::Struct::Instance::
operator[](T index)
{
  return rb_struct_aref(value(), ULONG2NUM(index));
}

template<>
inline Rice::Object Rice::Struct::Instance::
operator[]<Rice::Identifier>(Rice::Identifier member)
{
  unsigned long index = type_.offset_of(member);
  return (*this)[index];
}

template<>
inline Rice::Object Rice::Struct::Instance::
operator[]<char const *>(char const * name)
{
  return (*this)[Identifier(name)];
}
