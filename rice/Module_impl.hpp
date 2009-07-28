#ifndef Rice__Module_impl__hpp_
#define Rice__Module_impl__hpp_

#include "detail/Exception_Handler_defn.hpp"
#include "detail/ruby.hpp"
#include "Object_defn.hpp"
#include "Address_Registration_Guard_defn.hpp"
#include "Arg.hpp"

namespace Rice
{

class Module;
class Class;
template<typename T> class Data_Type;

/*! Holds all member data of Module_impl so it only exists in one place
 *  in the hierarchy.
 */
class Module_base
  : public Object
{
public:
  Module_base(VALUE v = rb_cObject);
  Module_base(Module_base const & other);

  Module_base & operator=(Module_base const & other);

  void swap(Module_base & other);

protected:
  template<typename Exception_T, typename Functor_T>
  void add_handler(Functor_T functor);

  Object handler() const;

private:
  Object mutable handler_;
  Address_Registration_Guard handler_guard_;
};

/*! An intermediate base class so we can always return the most-derived
 *  type (Module, Class, Data_Type, ...) without having to re-implement
 *  each function for each derived class.
 */
template<typename Base_T, typename Derived_T>
class Module_impl
  : public Base_T
{
public:
  Module_impl();

  template<typename T>
  Module_impl(T const & arg);

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
  Derived_T & add_handler(
      Functor_T functor);

  //! Define an instance method.
  /*! The method's implementation can be any function or member
   *  function.  A wrapper will be generated which will use from_ruby<>
   *  to convert the arguments from ruby types to C++ types before
   *  calling the function.  The return value will be converted back to
   *  ruby by using to_ruby().
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param arguments the list of arguments of this function, used for
   *  defining default parameters (optional)
   *  \return *this
   */
  template<typename Func_T>
  Derived_T & define_method(
      Identifier name,
      Func_T func,
      Arguments* arguments = 0);

  // FIXME There's GOT to be a better way to
  // do this. Handles the case where there is a single
  // argument defined for this method
  template<typename Func_T>
  Derived_T & define_method(
      Identifier name,
      Func_T func,
      Arg const& arg);

  //! Define a singleton method.
  /*! The method's implementation can be any function or member
   *  function.  A wrapper will be generated which will use from_ruby<>
   *  to convert the arguments from ruby types to C++ types before
   *  calling the function.  The return value will be converted back to
   *  ruby by using to_ruby().
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param arguments the list of arguments of this function, used for
   *  defining default parameters (optional)
   *  \return *this
   */
  template<typename Func_T>
  Derived_T & define_singleton_method(
      Identifier name,
      Func_T func,
      Arguments* arguments = 0);

  // FIXME: See define_method with Arg above
  template<typename Func_T>
  Derived_T & define_singleton_method(
      Identifier name,
      Func_T func,
      Arg const& arg);

  //! Define a module function.
  /*! A module function is a function that can be accessed either as a
   *  singleton method or as an instance method.
   *  The method's implementation can be any function or member
   *  function.  A wrapper will be generated which will use from_ruby<>
   *  to convert the arguments from ruby types to C++ types before
   *  calling the function.  The return value will be converted back to
   *  ruby by using to_ruby().
   *  \param name the name of the method
   *  \param func the implementation of the function, either a function
   *  pointer or a member function pointer.
   *  \param arguments the list of arguments of this function, used for
   *  defining default parameters (optional)
   *  \return *this
   */
  template<typename Func_T>
  Derived_T & define_module_function(
      Identifier name,
      Func_T func,
      Arguments* arguments = 0);

  // FIXME: See define_method with Arg above
  template<typename Func_T>
  Derived_T & define_module_function(
      Identifier name,
      Func_T func,
      Arg const& arg);

  //! Define an iterator.
  /*! Essentially this is a conversion from a C++-style begin/end
   *  iterator to a Ruby-style \#each iterator.
   *  \param begin a member function pointer to a function that returns
   *  an iterator to the beginning of the sequence.
   *  \param end a member function pointer to a function that returns an
   *  iterator to the end of the sequence.
   *  \param name the name of the iterator.
   *  \return *this
   */
  template<typename T, typename Iterator_T>
  Derived_T & define_iterator(
      Iterator_T (T::*begin)(),
      Iterator_T (T::*end)(),
      Identifier name = "each");

  //! Include a module.
  /*! \param inc the module to be included.
   *  \return *this
   */
  Derived_T & include_module(
      Module const & inc);

  //! Set a constant.
  /*! \param name the name of the constant to set.
   *  \param value the value of the constant.
   *  \return *this
   */
  Derived_T & const_set(
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

  //! Define a module under this module.
  /*! \param name the name of the module.
   *  \return the new class.
   */
  Module define_module(
      char const * name);

  //! Define a class under this module.
  /*! \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class(
      char const * name,
      Object superclass = rb_cObject);

  //! Define a new data class under this module.
  /*! The class will have a base class of Object.
   *  \param T the C++ type of the wrapped class.
   *  \return the new class.
   */
  // This function needs to be defined inline to work around a bug in
  // g++ 3.3.3.
  template<typename T>
  Data_Type<T>
  define_class(
      char const * name)
  {
    return this->define_class_with_object_as_base<T>(name);
  }

  //! Define a new data class under this module.
  /*! The class with have a base class determined by Base_T (specifically,
   *  Data_Type<Base_T>::klass).  Therefore, the type Base_T must already
   *  have been registered using define_class<> or define_class_under<>.
   *  \param T the C++ type of the wrapped class.
   *  \return the new class.
   */
  template<typename T, typename T_Base_T>
  Data_Type<T>
  define_class(
      char const * name);

private:
  // Workaround for g++ 3.3.3 (see above).
  template<typename T>
  Data_Type<T>
  define_class_with_object_as_base(
      char const * name);
};

} // namespace Rice

#endif // Rice__Module_impl__hpp_

