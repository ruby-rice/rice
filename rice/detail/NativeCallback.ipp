#include <array>
#include <algorithm>
#include <stdexcept>
#include <sstream>


namespace Rice::detail
{
  template<typename Return_T, typename ...Arg_Ts>
  Return_T NativeCallback<Return_T, Arg_Ts...>::call(Arg_Ts...args)
  {
    //VALUE result = protect(rb_yield_values, argCount, args...);
    Identifier id("call");
    std::array<VALUE, sizeof...(Arg_Ts)> values = { detail::To_Ruby<detail::remove_cv_recursive_t<Arg_Ts>>().convert(args)... };
    VALUE result = detail::protect(rb_funcallv, proc_, id.id(), (int)values.size(), (const VALUE*)values.data());

    // Can also use
    // rb_proc_call

    if constexpr (!std::is_void_v<Return_T>)
    {
      static From_Ruby<Return_T> fromRuby;
      return fromRuby.convert(result);
    }
  }
  
  // template<typename Return_T, template<typename ...> typename Irrelevant_T, typename ...Arg_Ts>
  //NativeCallback<Callback_T> NativeCallback<Callback_T>::define(VALUE proc)
  //void NativeCallback<Callback_T>::define(VALUE proc)
  //{
    // Have we defined this method yet in Ruby?
/*    Identifier identifier(method_name);
    const std::vector<std::unique_ptr<Native>>& natives = Registries::instance.natives.lookup(klass, identifier.id());
    if (natives.empty())
    {
      // Tell Ruby to invoke the static resolved method defined above
      detail::protect(rb_define_method, klass, method_name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);
    }

    // Create a NativeCallback instance and save it to the NativeRegistry. There may be multiple
    // NativeCallback instances for a specific method because C++ supports method overloading.
    NativeCallback_T* NativeCallback = new NativeCallback_T(klass, method_name, std::forward<Callback_T>(function), methodInfo);
    std::unique_ptr<Native> native(NativeCallback);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);*/
//  }
}
