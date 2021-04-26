// Include these methods to call methods from Module but return
// an instance of the current classes. This is an alternative to
// using CRTP.

auto& include_module(Module const& inc)
{
  detail::protect(rb_include_module, this->value(), inc.value());
  return *this;
}

auto& const_set(Identifier name, Object value)
{
  return dynamic_cast<decltype(*this)>(Module::const_set(name.id(), value.value()));
}

template<typename Function_T, typename...Arg_Ts>
auto& define_method(Identifier name, Function_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_method(name, std::forward<Function_T>(func), args...));
}

template<typename Function_T, typename...Arg_Ts>
auto& define_function(Identifier name, Function_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_function(name, std::forward<Function_T>(func), args...));
}

template<typename Function_T, typename...Arg_Ts>
auto& define_singleton_method(Identifier name, Function_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_method(name, std::forward<Function_T>(func), args...));
}

template<typename Function_T, typename...Arg_Ts>
auto& define_singleton_function(Identifier name, Function_T&& func, Arg_Ts const& ...args)
{
  return dynamic_cast<decltype(*this)>(Module::define_singleton_function(name, std::forward<Function_T>(func), args...));
}

template<typename Exception_T, typename Functor_T>
auto& add_handler(Functor_T functor)
{
  return dynamic_cast<decltype(*this)>(Module::add_handler<Exception_T>(functor));
}
