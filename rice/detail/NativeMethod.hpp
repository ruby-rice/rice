#ifndef Rice__detail__Native_Method__hpp_
#define Rice__detail__Native_Method__hpp_

namespace Rice::detail
{
  //! The NativeMethod class calls C++ functions/methods/lambdas on behalf of Ruby
  /*! The NativeMethod class is an intermediate between Ruby and C++. Every method
   *  defined in Rice is associated with a NativeFuntion instance that is stored in
   *  a unordered_map maintained by the MethodData class. The key is the Ruby class
   *  and method.
   * 
   *  When Ruby calls into C++ it invokes the static NativeMethod.call method. This
   *  method then looks up the NativeMethod instance and calls its ->() operator.
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
   *  @tparam Receiver_T - The type of C++ class wrapped by Ruby. Althought NativeMethod
   *    can derive the C++ class (Receiver_T), it can differ per member function. For example,
   *    std::map has a size() method but that is actually implemented on an ancestor class _Tree.
   *    Thus Receiver_T is std::map but Method_T::Receiver_T is _Tree. This breaks Rice in two ways. 
   *    First, _Tree is not a registered type. Second, Rice would return a _Tree instance back to
   *    C++ and not a std::map.
   *  @tparam Method_T - A template that represents the C++ function
   *    to call. This typename is automatically deduced by the compiler.
   *  @tparam IsMethod - A boolean specifying whether the function has
   *    a self parameter or not. Rice differentiates these two cases by
   *    calling them methods (self) or functions (no self).
   */

  template<typename Class_T, typename Method_T>
  class NativeMethod: Native
  {
  public:
    using NativeMethod_T = NativeMethod<Class_T, Method_T>;

    // We remove const to avoid an explosion of To_Ruby specializations and Ruby doesn't
    // have the concept of constants anyways
    using Return_T = typename method_traits<Method_T>::Return_T;
    using Receiver_T = typename method_traits<Method_T>::Class_T;
    using Parameter_Ts = typename method_traits<Method_T>::Parameter_Ts;
    using Apply_Args_T = typename tuple_unshift<Receiver_T, Parameter_Ts>::type;

    using To_Ruby_T = remove_cv_recursive_t<Return_T>;

    // Register method with Ruby
    static void define(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo);

  public:
    NativeMethod(VALUE klass, std::string method_name, Method_T method, MethodInfo* methodInfo);

    VALUE operator()(size_t argc, const VALUE* argv, VALUE self) override;
    std::string toString() override;

    std::string name() override;
    NativeKind kind() override;
    VALUE returnKlass() override;

  private:

    template<std::size_t...I>
    std::vector<std::string> argTypeNames(std::ostringstream& stream, std::index_sequence<I...>& indices);

    // Convert Ruby values to C++ values
    template<typename std::size_t...I>
    Apply_Args_T getNativeValues(VALUE self, std::vector<std::optional<VALUE>>& values, std::index_sequence<I...>& indices);

    // Figure out what self is
    Receiver_T getReceiver(VALUE self);

    // Throw an exception when wrapper cannot be extracted
    [[noreturn]] void noWrapper(const VALUE klass, const std::string& wrapper);

    // Do we need to keep alive any arguments?
    void checkKeepAlive(VALUE self, VALUE returnValue, std::vector<std::optional<VALUE>>& rubyValues);

    // Call the underlying C++ method
    VALUE invoke(VALUE self, Apply_Args_T&& nativeArgs);
    VALUE invokeNoGVL(VALUE self, Apply_Args_T&& nativeArgs);

  private:
    VALUE klass_;
    std::string method_name_;
    Method_T method_;
    std::unique_ptr<MethodInfo> methodInfo_;
    To_Ruby<To_Ruby_T> toRuby_;
  };
}

#endif // Rice__detail__Native_Method__hpp_
