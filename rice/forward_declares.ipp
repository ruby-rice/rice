#ifndef Rice__Forward_Declares__ipp_
#define Rice__Forward_Declares__ipp_

// These methods cannot be defined where they are declared due to circular dependencies
inline Rice::Class Rice::Object::
class_of() const
{
  return rb_class_of(value_);
}

inline Rice::String Rice::Object::
to_s() const
{
  return call("to_s");
}

inline Rice::String Rice::Object::
inspect() const
{
  return call("inspect");
}

inline Rice::Object Rice::Object::
instance_eval(String const& s)
{
  VALUE argv[] = { s.value() };
  return protect(rb_obj_instance_eval, 1, &argv[0], *this);
}

inline Rice::Object Rice::Object::
vcall(
  Identifier id,
  Array args)
{
  std::vector<VALUE> a(args.size());

  Array::const_iterator it = args.begin();
  Array::const_iterator end = args.end();

  for (int i = 0; it != end; i++, ++it)
  {
    a[i] = it->value();
  }

  return protect(rb_funcall3, *this, id, (int)args.size(), a.data());
}

inline std::ostream& Rice::
operator<<(std::ostream& out, Rice::Object const& obj)
{
  String s(obj.to_s());
  out << s.c_str();
  return out;
}

inline Rice::Identifier::
Identifier(Symbol const& symbol)
  : id_(SYM2ID(symbol.value()))
{
}

#endif Rice__Forward_Declares__ipp_