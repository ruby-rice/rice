#ifndef Rice__Module_defn__hpp_
#define Rice__Module_defn__hpp_

#include "detail/Exception_Handler_defn.hpp"
#include "detail/ruby.hpp"
#include "Object_defn.hpp"

namespace Rice
{
  template <typename T>
  void validateType();

  //! A helper for defining a Module and its methods.
  /*! This class provides a C++-style interface to ruby's Module class and
   *  for defining methods on that module.
   *
   *  Many of the methods are defined in Module_impl.hpp so that they can
   *  return a reference to the most derived type.
   */
   // TODO: we can't inherit from Builtin_Object, because Class needs
   // type T_CLASS and Module needs type T_MODULE
  class Module : public Object
  {
  public:
    //! Default construct a Module and initialize it to rb_cObject.
    Module();

    //! Construct a Module from an existing Module object.
    Module(VALUE v);

    //! Return the name of the module.
    String name() const;

    //! Return an array containing the Module's ancestors.
    /*! You will need to include Array.hpp to use this function.
     */
    Array ancestors() const;

    //! Return the module's singleton class.
    /*! You will need to include Class.hpp to use this function.
     */
    Class singleton_class() const;

    //! Define an exception handler.
    /*! Whenever an exception of type Exception_T is thrown from a
     *  function defined on this class, functor will be called to
     *  translate the exception into a ruby exception.
     *  \param Exception_T a template parameter indicating the type of
     *  exception to be translated.
     *  \param functor a functor to be called to translate the exception
     *  into a ruby exception.  This functor should re-throw the exception
     *  as an Exception.
     *  Example:
     *  \code
     *    class MyException : public std::exception { };
     *    Data_Type<MyException> rb_cMyException;
     *    Class rb_cFoo;
     *
     *    void translate_my_exception(MyException const & ex)
     *    {
     *      Data_Object<MyException> ex_(
     *          new MyException(ex),
     *          rb_cMyException);
     *      throw Exception(ex_);
     *    }
     *
     *    extern "C"
     *    void Init_MyExtension()
     *    {
     *      rb_cMyException = define_class("MyException");
     *      rb_cFoo = define_class("Foo")
     *        .add_handler<MyException>(translate_my_exception);
     *    }
     *  \endcode
     */
    template<typename Exception_T, typename Functor_T>
    Module& add_handler(Functor_T functor);

    //! Include a module.
    /*! \param inc the module to be included.
    *  \return *this
    */
    Module& include_module(Module const& inc);

    //! Define an instance method.
    /*! The method's implementation can be a member function, plain function
     *  or lambda. The easiest case is a member function where the Ruby
     *  method maps one-to-one to the C++ method. In the case of a
     *  plain function or lambda, the first argument must be SELF - ie, 
     *  the current object. If it is specified as a VALUE, then
     *  the current Ruby object is passed. If it is specified as a C++ class, 
     *  then the C++ oject is passed. If you don't want to include the 
     *  SELF argument see define_fuction.
     *  Rice will automatically convert method method from Ruby to C++ and
     *  then convert the return value from C++ to Ruby.
     *  \param name the name of the method
     *  \param func the implementation of the function, either a function
     *  pointer or a member function pointer.
     *  \param args a list of Arg instance used to define default parameters.
     *  \return *this
     */
    template<typename Function_T, typename...Arg_Ts>
    Module& define_method(Identifier name, Function_T&& func, const Arg_Ts& ...args);

    //! Define an instance function.
    /*! The function implementation is a plain function or a static
     *  member function. 
     *  Rice will automatically convert method method from Ruby to C++ and
     *  then convert the return value from C++ to Ruby.
     *  \param name the name of the method
     *  \param func the implementation of the function, either a function
     *  pointer or a member function pointer.
     *  \param args a list of Arg instance used to define default parameters (optional)
     *  \return *this
     */
    template<typename Function_T, typename...Arg_Ts>
    Module& define_function(Identifier name, Function_T&& func, const Arg_Ts& ...args);

    //! Define a singleton method.
    /*! The method's implementation can be a static member function,
    *   plain function or lambda. In all cases the first argument 
    *   must be SELF - ie, he current object. If it is specified as a VALUE, then
     *  the current Ruby object is passed. If it is specified as a C++ class, 
     *  then the C++ oject is passed. If you don't want to include the 
     *  SELF argument see define_singleton_function.
     *  Rice will automatically convert method method from Ruby to C++ and
     *  then convert the return value from C++ to Ruby.
     *  \param name the name of the method
     *  \param func the implementation of the function, either a function
     *  pointer or a member function pointer.
     *  \param args a list of Arg instance used to define default parameters (optional)
     *  \return *this
     */
    template<typename Function_T, typename...Arg_Ts>
    Module& define_singleton_method(Identifier name, Function_T&& func, const Arg_Ts& ...args);

    //! Define a singleton method.
    /*! The method's implementation can be a static member function, plain
     *  function or lambda.
     . A wrapper will be 
     * generated which will convert the method
     *  from ruby types to C++ types before calling the function.  The return
     *  value will be converted back to ruby.
     *  \param name the name of the method
     *  \param func the implementation of the function, either a function
     *  pointer or a member function pointer.
     *  \param args a list of Arg instance used to define default parameters (optional)
     *  \return *this
     */
    template<typename Function_T, typename...Arg_Ts>
    Module& define_singleton_function(Identifier name, Function_T&& func, const Arg_Ts& ...args);

    //! Define a module function.
    /*! A module function is a function that can be accessed either as a
     *  singleton method or as an instance method. It wrap a plain
     *  function, static member function or lambda.
     *  Rice will automatically convert method method from Ruby to C++ and
     *  then convert the return value from C++ to Ruby.
     *  \param name the name of the method
     *  \param func the implementation of the function, either a function
     *  pointer or a member function pointer.
     *  \param args a list of Arg instance used to define default parameters (optional)
     *  \return *this
     */
    template<typename Function_T, typename...Arg_Ts>
    Module& define_module_function(Identifier name, Function_T&& func, const Arg_Ts& ...args);

    //! Set a constant.
    /*! \param name the name of the constant to set.
     *  \param value the value of the constant.
     *  \return *this
     */
    Module& const_set(Identifier name, Object value);

    //! Get a constant.
    /*! \param name the name of the constant to get.
     *  \return the value of the constant.
     */
    Object const_get(Identifier name) const;

    //! Determine whether a constant is defined.
    /*! \param name the name of the constant to check.
     *  \return true if the constant is defined in this module or false
     *  otherwise.
     */
    bool const_defined(Identifier name) const;

    //! Remove a constant.
    /*! \param name the name of the constant to remove.
     */
    void remove_const(Identifier name);

  protected:
    std::shared_ptr<detail::Exception_Handler> handler() const;

  private:
    template<typename Function_T>
    void wrap_native_method(VALUE klass, Identifier name, Function_T&& function,
      std::shared_ptr<detail::Exception_Handler> handler, MethodInfo* methodInfo);

    template<typename Function_T>
    void wrap_native_function(VALUE klass, Identifier name, Function_T&& function,
      std::shared_ptr<detail::Exception_Handler> handler, MethodInfo* methodInfo);

    mutable std::shared_ptr<detail::Exception_Handler> handler_ = std::make_shared<Rice::detail::Default_Exception_Handler>();
  };

  //! Define a new module in the namespace given by module.
  /*! \param module the module in which to define the new module.
   *  \param name the name of the new module.
   */
  Module define_module_under(Object module, char const * name);

  //! Define a new module in the default namespace.
  /*! \param name the name of the new module.
   */
  Module define_module(char const * name);

  //! Create a new anonymous module.
  /*! \return the new module.
   */
  Module anonymous_module();
}
#endif // Rice__Module_defn__hpp_