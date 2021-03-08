#ifndef Rice__Module_defn__hpp_
#define Rice__Module_defn__hpp_

#include "detail/Exception_Handler_defn.hpp"
#include "Arg.hpp"
#include "Object_defn.hpp"

namespace Rice
{
template<typename T>
class Data_Type;

//! A helper for defining a Module and its methods.
/*! This class provides a C++-style interface to ruby's Module class and
 *  for defining methods on that module.
 *
 *  Many of the methods are defined in Module_impl.hpp so that they can
 *  return a reference to the most derived type.
 */
class Module
  // TODO: we can't inherit from Builtin_Object, because Class needs
  // type T_CLASS and Module needs type T_MODULE
  : public Object
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

   //! Define a class under this module.
  /*! \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class(char const* name, Object superclass = rb_cObject);

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
  Module& add_handler(
    Functor_T functor);

  //! Include a module.
  /*! \param inc the module to be included.
  *  \return *this
  */
  Module& include_module(Module const& inc);

  template<typename Func_T>
  [[deprecated("Please call define_method with Arg parameters")]]
  Module& define_method(
    Identifier name,
    Func_T&& func,
    Arguments* arguments);

  //! Define an instance method.
  /*! The method's implementation can be a member function, plain function
   *  or lambda. The easiest case is a member function where the Ruby
   *  arguments map one-to-one to the C++ arguments. In the case of a
   *  plain function or lambda, the first argument must be SELF - ie, 
   *  the current object. If it is specified as a VALUE, then
   *  the current Ruby object is passed. If it is specified as a C++ class, 
   *  then the C++ oject is passed. If you don't want to include the 
   *  SELF argument see define_fuction.
   *  Rice will automatically convert method arguments from Ruby to C++ and
   *  then convert the return value from C++ to Ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters.
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_method(
    Identifier name,
    Func_T&& func,
    Arg_Ts const& ...args);

  //! Define an instance function.
  /*! The function implementation is a plain function or a static
   *  member function. 
   *  Rice will automatically convert method arguments from Ruby to C++ and
   *  then convert the return value from C++ to Ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_function(
    Identifier name,
    Func_T&& func,
    Arg_Ts const& ...args);

  template<typename Func_T>
  [[deprecated("Please call define_singleton_method with Arg parameters")]]
  Module& define_singleton_method(
    Identifier name,
    Func_T&& func,
    Arguments* arguments);

  //! Define a singleton method.
  /*! The method's implementation can be a static member function,
  *   plain function or lambda. In all cases the first argument 
  *   must be SELF - ie, he current object. If it is specified as a VALUE, then
   *  the current Ruby object is passed. If it is specified as a C++ class, 
   *  then the C++ oject is passed. If you don't want to include the 
   *  SELF argument see define_singleton_function.
   *  Rice will automatically convert method arguments from Ruby to C++ and
   *  then convert the return value from C++ to Ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_singleton_method(
    Identifier name,
    Func_T&& func,
    Arg_Ts const& ...args);

  //! Define a singleton method.
  /*! The method's implementation can be a static member function, plain
   *  function or lambda.
   . A wrapper will be 
   * generated which will convert the arguments
   *  from ruby types to C++ types before calling the function.  The return
   *  value will be converted back to ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_singleton_function(
    Identifier name,
    Func_T&& func,
    Arg_Ts const& ...args);

  template<typename Func_T>
  [[deprecated("Please call define_module_function with Arg parameters")]]
  Module& define_module_function(
    Identifier name,
    Func_T&& func,
    Arguments* arguments);

  //! Define a module function.
  /*! A module function is a function that can be accessed either as a
   *  singleton method or as an instance method. It wrap a plain
   *  function, static member function or lambda.
   *  Rice will automatically convert method arguments from Ruby to C++ and
   *  then convert the return value from C++ to Ruby.
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *  \return *this
   */
  template<typename Func_T, typename...Arg_Ts>
  Module& define_module_function(
    Identifier name,
    Func_T&& func,
    Arg_Ts const& ...args);

  //! Set a constant.
  /*! \param name the name of the constant to set.
   *  \param value the value of the constant.
   *  \return *this
   */
  Module& const_set(
    Identifier name,
    Object value);

  //! Get a constant.
  /*! \param name the name of the constant to get.
   *  \return the value of the constant.
   */
  Object const_get(
    Identifier name) const;

  //! Determine whether a constant is defined.
  /*! \param name the name of the constant to check.
   *  \return true if the constant is defined in this module or false
   *  otherwise.
   */
  bool const_defined(
    Identifier name) const;

  //! Remove a constant.
  /*! \param name the name of the constant to remove.
   */
  void remove_const(
    Identifier name);

  //! Define a new data class under this module.
  /*! The class will have a base class of Object.
   *  \param T the C++ type of the wrapped class.
   *  \return the new class.
   */
  template<typename T>
  Data_Type<T>
    define_class(
      char const* name);

  //! Define a new data class under this module.
  /*! The class with have a base class determined by Intrinsic_T (specifically,
   *  Data_Type<Intrinsic_T>::klass).  Therefore, the type Intrinsic_T must already
   *  have been registered using define_class<> or define_class_under<>.
   *  \param T the C++ type of the wrapped class.
   *  \return the new class.
   */
  template<typename T, typename T_Base_T>
  Data_Type<T>
    define_class(
      char const* name);

protected:
  std::shared_ptr<detail::Exception_Handler> handler() const;

private:
  template<typename T>
  Data_Type<T> define_class_with_object_as_base(char const* name);

  template<typename Function_T>
  void wrap_native_method(VALUE klass, Identifier name, Function_T&& function,
    std::shared_ptr<detail::Exception_Handler> handler, Arguments* arguments = 0);

  template<typename Function_T>
  void wrap_native_function(VALUE klass, Identifier name, Function_T&& function,
    std::shared_ptr<detail::Exception_Handler> handler, Arguments* arguments = 0);

  mutable std::shared_ptr<detail::Exception_Handler> handler_ = std::make_shared<Rice::detail::Default_Exception_Handler>();
};

//! Define a new module in the namespace given by module.
/*! \param module the module in which to define the new module.
 *  \param name the name of the new module.
 */
Module define_module_under(
    Object module,
    char const * name);

//! Define a new module in the default namespace.
/*! \param name the name of the new module.
 */
Module define_module(
    char const * name);

//! Create a new anonymous module.
/*! \return the new module.
 */
Module anonymous_module();

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Module>
{
  static Rice::Module convert(VALUE value)
  {
    return Rice::Module(value);
  }
};

#endif // Rice__Module_defn__hpp_

