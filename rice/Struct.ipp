namespace Rice
{

Struct::
Struct()
  : Module_impl<Class, Struct>(rb_cObject)
  , members_()
  , members_guard_(&members_)
  , member_offset_()
  , member_offset_guard_(&member_offset_)
{
}

Struct::
Struct(Struct const & s)
  : Module_impl<Class, Struct>(s)
  , members_(s.members_.value())
  , members_guard_(&members_)
  , member_offset_(s.member_offset_.value())
  , member_offset_guard_(&member_offset_)
{
}

Struct::
~Struct()
{
}

Struct & Struct::
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

Struct & Struct::
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

size_t Struct::
offset_of(Identifier name) const
{
  Symbol ruby_name(name);
  return from_ruby<size_t>(member_offset_[ruby_name]);
}

void Struct::
swap(Struct & other)
{
  members_.swap(other.members_);
  members_guard_.swap(other.members_guard_);
  member_offset_.swap(other.member_offset_);
  member_offset_guard_.swap(other.member_offset_guard_);
  Class::swap(other);
}

Struct::Instance
Struct::
new_instance(Array args) const
{
  Object instance = const_cast<Struct *>(this)->vcall("new", args);
  return Instance(*this, instance);
}

Struct::Instance::
Instance(
    Struct const & type,
    Array args)
  : Builtin_Object<T_STRUCT>(type.new_instance(args))
  , type_(type)
{
}

Struct::Instance::
Instance(
    Struct const & type,
    Object s)
  : Builtin_Object<T_STRUCT>(s)
  , type_(type)
{
}

void Struct::Instance::
swap(Instance & other)
{
  type_.swap(other.type_);
  Builtin_Object<T_STRUCT>::swap(other);
}

template<typename T>
inline Object Struct::Instance::
operator[](T index)
{
  return rb_struct_aref(value(), ULONG2NUM(index));
}

template<>
inline Object Struct::Instance::
operator[]<Identifier>(Identifier member)
{
  size_t index = type_.offset_of(member);
  return (*this)[index];
}

template<>
inline Object Struct::Instance::
operator[]<char const *>(char const * name)
{
  return (*this)[Identifier(name)];
}


Struct
define_struct()
{
  return Struct();
}

} // Rice
