namespace Rice
{

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

} // Rice
