#ifndef Rice__Forward_Declares__ipp_
#define Rice__Forward_Declares__ipp_

namespace Rice
{
  // These methods cannot be defined where they are declared due to circular dependencies
  inline Class Object::class_of() const
  {
    return rb_class_of(value_);
  }

  inline String Object::to_s() const
  {
    return call("to_s");
  }

  inline String Object::inspect() const
  {
    return call("inspect");
  }

  inline Object Object::instance_eval(String const& s)
  {
    const VALUE argv[] = { s.value() };
    return detail::protect(rb_obj_instance_eval, 1, &argv[0], this->value());
  }

  inline Object Object::vcall(Identifier id, Array args)
  {
    std::vector<VALUE> a(args.size());

    Array::const_iterator it = args.begin();
    Array::const_iterator end = args.end();

    for (int i = 0; it != end; i++, ++it)
    {
      a[i] = it->value();
    }

    return detail::protect(rb_funcall3, this->value(), id.id(), (int)args.size(), (const VALUE*)a.data());
  }

  inline std::ostream& operator<<(std::ostream& out, Object const& obj)
  {
    String s(obj.to_s());
    out << s.c_str();
    return out;
  }

  inline Identifier::Identifier(Symbol const& symbol)
    : id_(SYM2ID(symbol.value()))
  {
  }

  inline String Module::name() const
  {
    return rb_mod_name(this->value());
  }

  inline Array Module::ancestors() const
  {
    return detail::protect(rb_mod_ancestors, this->value());
  }

  inline Class Module::singleton_class() const
  {
    return CLASS_OF(value());
  }

  template<typename Function_T>
  inline void Module::wrap_native_method(VALUE klass, Identifier name, Function_T&& function,
    std::shared_ptr<detail::Exception_Handler> handler, Arguments* arguments)
  {
    auto* native = detail::Make_Native_Function_With_Self(std::forward<Function_T>(function), handler, arguments);
    using Native_T = typename std::remove_pointer_t<decltype(native)>;

    detail::verifyType<typename Native_T::Native_Return_T>();
    detail::verifyTypes<typename Native_T::Native_Arg_Ts>();

    detail::MethodData::define_method(klass, name.id(), &Native_T::call, -1, native);
  }

  template<typename Function_T>
  inline void Module::wrap_native_function(VALUE klass, Identifier name, Function_T&& function,
    std::shared_ptr<detail::Exception_Handler> handler, Arguments* arguments)
  {
    auto* native = detail::Make_Native_Function(std::forward<Function_T>(function), handler, arguments);
    using Native_T = typename std::remove_pointer_t<decltype(native)>;

    detail::verifyType<typename Native_T::Native_Return_T>();
    detail::verifyTypes<typename Native_T::Native_Arg_Ts>();

    detail::MethodData::define_method(klass, name.id(), &Native_T::call, -1, native);
  }
}
#endif // Rice__Forward_Declares__ipp_
