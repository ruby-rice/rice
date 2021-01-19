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

inline
Rice::String Rice::Module::
name() const
{
  Object name = rb_mod_name(*this);
  if (name.is_nil())
  {
    // 1.9
    return String("");
  }
  else
  {
    return name;
  }
}

inline
Rice::Array
Rice::Module::
ancestors() const
{
  return protect(rb_mod_ancestors, *this);
}

inline Rice::Class
Rice::Module::
singleton_class() const
{
  return CLASS_OF(value());
}

inline
Rice::Class
Rice::Module::
define_class(
  char const* name,
  Object superclass)
{
  return Rice::define_class_under(*this, name, superclass);
}

template<typename T>
inline
Rice::Data_Type<T>
Rice::Module::
define_class_with_object_as_base(
  char const* name)
{
  return Rice::define_class_under<T>(*this, name);
}

template<typename T, typename T_Base_T>
inline
Rice::Data_Type<T>
Rice::Module::
define_class(
  char const* name)
{
  return Rice::define_class_under<T, T_Base_T>(
    *this,
    name);
}

template<typename Fun_T>
inline void Rice::Module::define_method_and_auto_wrap(VALUE klass, Identifier name, Fun_T function,
  std::shared_ptr<detail::Exception_Handler> handler,
  Arguments* arguments)
{
  auto* wrapper = detail::wrap_function(function, handler, arguments);
  using WrapperType = std::remove_pointer_t<decltype(wrapper)>;

  Data_Object<WrapperType> f(wrapper, rb_cObject);

  Rice::protect(
    detail::define_method_with_data,
    klass,
    name.id(),
    RUBY_METHOD_FUNC(&WrapperType::call),
    -1,
    f);
}

#endif // Rice__Forward_Declares__ipp_