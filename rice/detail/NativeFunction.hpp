#ifndef Rice__detail__Native_Function__hpp_
#define Rice__detail__Native_Function__hpp_

#include "ruby.hpp"
#include "Exception_Handler_defn.hpp"
#include "MethodInfo.hpp"
#include "function_traits.hpp"
#include "from_ruby.hpp"

namespace Rice::detail
{
  template<typename Function_T, bool IsMethod>
  class NativeFunction
  {
  public:
    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = remove_cv_recursive_t<typename function_traits<Function_T>::return_type>;
    using Self_T = typename method_traits<Function_T, IsMethod>::Self_T;
    using Arg_Ts = typename method_traits<Function_T, IsMethod>::Arg_Ts;
    using From_Ruby_Ts = typename tuple_map<From_Ruby, Arg_Ts>::type;

    // Static member function that Ruby calls
    static VALUE call(int argc, VALUE* argv, VALUE self);

  public:
    NativeFunction(Function_T func, std::shared_ptr<Exception_Handler> handler, MethodInfo* methodInfo);

    // Invokes the wrapped function
    VALUE operator()(int argc, VALUE* argv, VALUE self);

  private:
    template<typename T, std::size_t I>
    From_Ruby<T> createFromRuby();
      
    // Create NativeArgs which are used to convert values from Ruby to C++
    template<std::size_t...I>
    From_Ruby_Ts createFromRuby(std::index_sequence<I...>& indices);

    To_Ruby<Return_T> createToRuby();
      
    // Convert Ruby argv pointer to Ruby values
    std::vector<VALUE> getRubyValues(int argc, VALUE* argv);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Arg_Ts getNativeValues(std::vector<VALUE>& values, std::index_sequence<I...>& indices);

    // Figure out what self is
    Self_T getSelf(VALUE self);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<VALUE>& rubyValues);

    // Call the underlying C++ function
    VALUE invokeNativeFunction(Arg_Ts& nativeArgs);
    VALUE invokeNativeMethod(VALUE self, Arg_Ts& nativeArgs);

  private:
    Function_T func_;
    From_Ruby_Ts fromRubys_;
    To_Ruby<Return_T> toRuby_;
    std::shared_ptr<Exception_Handler> handler_;
    std::unique_ptr<MethodInfo> methodInfo_;
  };
}
#include "NativeFunction.ipp"

#endif // Rice__detail__Native_Function__hpp_