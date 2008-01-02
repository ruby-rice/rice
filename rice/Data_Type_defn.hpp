#ifndef Rice__Data_Type_defn__hpp_
#define Rice__Data_Type_defn__hpp_

#include "Class_defn.hpp"
#include "Data_Type_fwd.hpp"
#include "detail/ruby.hpp"
#include <memory>
#include <map>
#include <set>

namespace Rice
{

namespace detail
{
  class Abstract_Caster;
}

class Module;

//! The base class for all instantiations of Data_Type.
class Data_Type_Base
  : public Module_impl<Class, Data_Type_Base>
{
public:
  //! Default constructor.
  Data_Type_Base();

  //! Constructor.
  Data_Type_Base(VALUE v);

  //! Destructor.
  virtual ~Data_Type_Base() = 0;

protected:
  virtual detail::Abstract_Caster * caster() const = 0;

  typedef std::map<VALUE, detail::Abstract_Caster *> Casters;
  static Casters casters_;
};

//! A mechanism for binding ruby types to C++ types.
/*! This class binds run-time types (Ruby VALUEs) to compile-time types
 *  (C++ types).  The binding can occur only once.
 */
template<typename T>
class Data_Type
  : public Module_impl<Data_Type_Base, Data_Type<T> >
{
public:
  //! The C++ type being held.
  typedef T Type;

  //! Default constructor which does not bind.
  /*! No member functions must be called on this Data_Type except bind,
   *  until the type is bound.
   */
  Data_Type();

  //! Constructor which takes a Module.
  /*! Binds the type to the given VALUE according to the rules given
   *  above.
   *  \param klass the module to which to bind.
   */
  Data_Type(Module const & v);

  //! Destructor.
  virtual ~Data_Type();
 
  //! Explictly return the Ruby type.
  /*! \return the ruby class to which the type is bound.
   */
  static Module klass() { return klass_; }

  //! Assignment operator which takes a Module
  /*! \param klass must be the class to which this data type is already
   *  bound.
   *  \return *this
   */
  virtual Data_Type & operator=(Module const & klass);

  //! Define a constructor for the class.
  /*! Creates a singleton method allocate and an instance method called
   *  initialize which together create a new instance of the class.  The
   *  allocate method allocates memory for the object reference and the
   *  initialize method constructs the object.
   *  \param constructor an object that has a static member function
   *  construct() that constructs a new instance of T and sets the object's data
   *  member to point to the new instance.  A helper class Constructor
   *  is provided that does precisely this.
   *  For example:
   *  \code
   *    define_class<Foo>("Foo")
   *      .define_constructor(Constructor<Foo>());
   *  \endcode
   */
  template<typename Constructor_T>
  Data_Type<T> & define_constructor(
      Constructor_T constructor);

  //! Convert ruby object x to type T.
  /*! \param x the object to convert.
   *  \return the C++ object wrapped inside object x.
   */
  static T * from_ruby(Object x);

  //! Determine if the type is bound.
  /*! \return true if the object is bound, false otherwise.
   */
  static bool is_bound();

protected:
  //! Bind a Data_Type to a VALUE.
  /*! Throws an exception if the Data_Type is already bound to a
   *  different class.  Any existing instances of the Data_Type will be
   *  bound after this function returns.
   *  \param klass the ruby type to which to bind.
   *  \return *this
   */
  template<typename Base_T>
  static Data_Type bind(Module const & klass);

  template<typename T_>
  friend Rice::Data_Type<T_> define_class_under(
      Object module,
      char const * name);

  template<typename T_, typename Base_T_>
  friend Rice::Data_Type<T_> define_class_under(
      Object module,
      char const * name);

  template<typename T_>
  friend Rice::Data_Type<T_> Rice::define_class(
      char const * name);

  template<typename T_, typename Base_T_>
  friend Rice::Data_Type<T_> define_class(
      char const * name);

private:
  template<typename T_>
  friend class Data_Type;

  virtual detail::Abstract_Caster * caster() const;

  static void check_is_bound();

  static VALUE klass_;
  static std::auto_ptr<detail::Abstract_Caster> caster_;

  typedef std::set<Data_Type<T> *> Instances;

  static Instances & unbound_instances()
  {
    static Instances unbound_instances;
    return unbound_instances;
  }
};

//! Define a new data class in the namespace given by module.
/*! The class will have a base class of Object.
 *  \param T the C++ type of the wrapped class.
 *  \param module the the Module in which to define the class.
 *  \return the new class.
 */
template<typename T>
Rice::Data_Type<T> define_class_under(
    Object module,
    char const * name);

//! Define a new data class in the namespace given by module.
/*! The class with have a base class determined by Base_T (specifically,
 *  Data_Type<Base_T>::klass).  Therefore, the type Base_T must already
 *  have been registered using define_class<> or define_class_under<>.
 *  \param T the C++ type of the wrapped class.
 *  \param module the the Module in which to define the class.
 *  \return the new class.
 */
template<typename T, typename Base_T>
Rice::Data_Type<T> define_class_under(
    Object module,
    char const * name);

//! Define a new data class in the default namespace.
/*! The class will have a base class of Object.
 *  \param T the C++ type of the wrapped class.
 *  \return the new class.
 */
template<typename T>
Rice::Data_Type<T> define_class(
    char const * name);

//! Define a new data class in the default namespace.
/*! The class with have a base class determined by Base_T (specifically,
 *  Data_Type<Base_T>::klass).  Therefore, the type Base_T must already
 *  have been registered using define_class<> or define_class_under<>.
 *  \param T the C++ type of the wrapped class.
 *  \param module the the Module in which to define the class.
 *  \return the new class.
 */
template<typename T, typename Base_T>
Rice::Data_Type<T> define_class(
    char const * name);

} // namespace Rice

#include "Data_Type.ipp"

#endif // Rice__Data_Type_defn__hpp_

