// Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.

auto& include_module(Module const& inc)
{
  protect(rb_include_module, *this, inc);
  return *this;
}

auto& const_set(Identifier name, Object value)
{
  return dynamic_cast<decltype(*this)>(Module::const_set(name, value));
}

template<typename Func_T>
auto& define_method(Identifier name, Func_T func, Arguments* arguments = 0)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, func, arguments));
}

template<typename Func_T>
auto& define_method(Identifier name, Func_T func, Arg const& arg)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, func, arg));
}

template<typename Func_T>
auto& define_singleton_method(Identifier name, Func_T func, Arguments* arguments = 0)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, func, arguments));
}

template<typename Func_T>
auto& define_singleton_method(Identifier name, Func_T func, Arg const& arg)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, func, arg));
}
