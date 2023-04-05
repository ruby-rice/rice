#ifndef Rice__Module_defn__hpp_
#define Rice__Module_defn__hpp_

#include "../detail/Exception_Handler_defn.hpp"
#include "../detail/ruby.hpp"
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

    //! Construct a Module from an string that references a Module
    Module(std::string name, Object under = rb_cObject);

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

    //! Evaluate the given string in the context of the module.
    /*! This is equivalant to calling obj.module_eval(s) from inside the
     *  interpreter.
     *  \return the result of the expression.
     */
    Object module_eval(String const& s);

    #include "shared_methods.hpp"

  protected:
    std::shared_ptr<detail::Exception_Handler> handler() const;

    template<bool IsMethod, typename Function_T>
    void wrap_native_call(VALUE klass, Identifier name, Function_T&& function,
                          std::shared_ptr<detail::Exception_Handler> handler, MethodInfo* methodInfo);

  private:
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