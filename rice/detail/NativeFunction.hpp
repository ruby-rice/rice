#ifndef Rice__detail__Native_Function__hpp_
#define Rice__detail__Native_Function__hpp_

namespace Rice::detail
{
  //! The NativeFunction class calls C++ functions/methods/lambdas on behalf of Ruby
  /*! The NativeFunction class is an intermediate between Ruby and C++. Every method
   *  defined in Rice is associated with a NativeFuntion instance that is stored in
   *  a unordered_map maintained by the MethodData class. The key is the Ruby class
   *  and method.
   * 
   *  When Ruby calls into C++ it invokes the static NativeFunction.call method. This
   *  method then looks up the NativeFunction instance and calls its ->() operator.
   *
   *  The instance then converts each of the arguments passed from Ruby into their
   *  C++ equivalents. It then retrieves the C++ object (if there is one, Ruby could
   *  be calling a free standing method or lambda). Then it calls the C++ method
   *  and gets back the result. If there is a result (so not void), it is converted
   *  from a C++ object to a Ruby object and returned back to Ruby.
   * 
   *  This class make heavy use of C++ Template metaprogramming to determine
   *  the types and parameters a method takes. It then uses that information
   *  to perform type conversion Ruby to C++.
   *   
   *  @tparam Receiver_T - The type of C++ class wrapped by Ruby. Althought NativeFunction
   *    can derive the C++ class (Receiver_T), it can differ per member function. For example,
   *    std::map has a size() method but that is actually implemented on an ancestor class _Tree.
   *    Thus Receiver_T is std::map but Function_T::Receiver_T is _Tree. This breaks Rice in two ways. 
   *    First, _Tree is not a registered type. Second, Rice would return a _Tree instance back to
   *    C++ and not a std::map.
   *  @tparam Function_T - A template that represents the C++ function
   *    to call. This typename is automatically deduced by the compiler.
   *  @tparam IsMethod - A boolean specifying whether the function has
   *    a self parameter or not. Rice differentiates these two cases by
   *    calling them methods (self) or functions (no self).
   */

  template<typename Class_T, typename Function_T, bool IsMethod>
  class NativeFunction: Native
  {
  public:
    using NativeFunction_T = NativeFunction<Class_T, Function_T, IsMethod>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename function_traits<Function_T>::return_type;
    using Receiver_T = typename method_traits<Function_T, IsMethod>::Class_T;
    using Arg_Ts = typename method_traits<Function_T, IsMethod>::Arg_Ts;
    static constexpr std::size_t arity = method_traits<Function_T, IsMethod>::arity;
    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Register function with Ruby
    static void define(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo);

    // Proc entry
    static VALUE procEntry(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
    static VALUE finalizerCallback(VALUE yielded_arg, VALUE callback_arg, int argc, const VALUE* argv, VALUE blockarg);
 
  public:
    NativeFunction(Function_T function);
    NativeFunction(VALUE klass, std::string method_name, Function_T function, MethodInfo* methodInfo);

    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;

  private:

    template<std::size_t...I>
    std::vector<std::string> argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices);

    // Convert C++ value to Ruby
    To_Ruby<To_Ruby_T> createToRuby();
      
    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Arg_Ts getNativeValues(std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices);

    // Figure out what self is
    Receiver_T getReceiver(VALUE self);

    // Throw an exception when wrapper cannot be extracted
    [[noreturn]] void noWrapper(const VALUE klass, const std::string& wrapper);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues);

    // Call the underlying C++ function
    VALUE invokeNativeFunction(Arg_Ts&& nativeArgs);
    VALUE invokeNativeMethod(VALUE self, Arg_Ts&& nativeArgs);

  private:
    VALUE klass_;
    std::string method_name_;
    Function_T function_;
    To_Ruby<To_Ruby_T> toRuby_;
    std::unique_ptr<MethodInfo> methodInfo_;
  };
}

#endif // Rice__detail__Native_Function__hpp_
