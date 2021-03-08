// Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.

auto& include_module(Module const& inc)
{
  detail::protect(rb_include_module, *this, inc);
  return *this;
}

auto& const_set(Identifier name, Object value)
{
  return dynamic_cast<decltype(*this)>(Module::const_set(name, value));
}

template<typename Func_T>
[[deprecated("Please call define_method with Arg parameters")]]
auto& define_method(Identifier name, Func_T&& func, Arguments* arguments)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, std::forward<Func_T>(func), arguments));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_method(Identifier name, Func_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, std::forward<Func_T>(func), args...));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_function(Identifier name, Func_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_function(name, std::forward<Func_T>(func), args...));
}

template<typename Func_T>
[[deprecated("Please call define_singleton_method with Arg parameters")]]
auto& define_singleton_method(Identifier name, Func_T&& func, Arguments* arguments)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, std::forward<Func_T>(func), arguments));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_singleton_method(Identifier name, Func_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, std::forward<Func_T>(func), args...));
}

template<typename Func_T, typename...Arg_Ts>
auto& define_singleton_function(Identifier name, Func_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_function(name, std::forward<Func_T>(func), args...));
}