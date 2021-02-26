#ifndef Rice__Data_Type_defn__hpp_
#define Rice__Data_Type_defn__hpp_

#include "Class_defn.hpp"
#include "detail/ruby.hpp"

#include <memory>
#include <map>
#include <set>

/*!
 *  \example map/map.cpp
 */

namespace Rice
{

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

//! Define an implicit conversion rule between two types.
/*! Given two types, which can be custom types already
 *  wrapped into Rice or fundamental C++ types, this
 *  tells Rice that the two types can be used interchangably.
 *  \param From_T The type to convert from
 *  \param To_T The type to convert to
 */
template<typename From_T, typename To_T>
void define_implicit_cast();

//! A mechanism for binding ruby types to C++ types.
/*! This class binds run-time types (Ruby VALUEs) to compile-time types
 *  (C++ types).  The binding can occur only once.
 */
template<typename T>
class Data_Type
  : public Class
{
  static_assert(!std::is_pointer_v<T>);
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_const_v<T>);
  static_assert(!std::is_volatile_v<T>);

public:
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
 
  //! Return the Ruby class.
  /*! \return the ruby class to which the type is bound.
   */
  static Module klass();

  //! Return the Ruby type.
  static rb_data_type_t* rb_type();

  //! Assignment operator which takes a Module
  /*! \param klass must be the class to which this data type is already
   *  bound.
   *  \return *this
   */
  virtual Data_Type & operator=(Module const & klass);

  //! Define a constructor for the class.
  template<typename Constructor_T>
  [[deprecated("Please call define_constructor with Arg parameters")]]
  Data_Type<T> & define_constructor(
      Constructor_T constructor,
      Arguments * arguments);

  /*! Creates a singleton method allocate and an instance method called
   *  initialize which together create a new instance of the class.  The
   *  allocate method allocates memory for the object reference and the
   *  initialize method constructs the object.
   *  \param constructor an object that has a static member function
   *  construct() that constructs a new instance of T and sets the object's data
   *  member to point to the new instance.  A helper class Constructor
   *  is provided that does precisely this.
   *  \param args a list of Arg instance used to define default parameters (optional)
   *
   *  For example:
   *  \code
   *    define_class<Foo>("Foo")
   *      .define_constructor(Constructor<Foo>());
   *  \endcode
   */
  template<typename Constructor_T, typename...Arg_Ts>
  Data_Type<T> & define_constructor(
      Constructor_T constructor,
      Arg_Ts const& ...args);

  //! Register a Director class for this class.
  /*! For any class that uses Rice::Director to enable polymorphism
   *  across the languages, you need to register that director proxy
   *  class with this method. Not doing so will cause the resulting 
   *  library to die at run time when it tries to convert the base
   *  type into the Director proxy type.
   *
   *  This method takes no arguments, just needs the type of the
   *  Director proxy class.
   *
   *  For example:
   *  \code
   *    class FooDirector : public Foo, public Rice::Director {
   *      ...
   *    };
   *
   *    define_class<Foo>("Foo")
   *      .define_director<FooDirector>()
   *      .define_constructor(Constructor<FooDirector, Rice::Object>());
   *  \endcode
   */
  template<typename Director_T>
  Data_Type<T>& define_director();

  //! Determine if the type is bound.
  /*! \return true if the object is bound, false otherwise.
   */
  static bool is_bound();
  static void check_is_bound();
  static bool is_descendant(VALUE value);
  
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

  template<typename U = T, typename Iterator_Return_T>
  Data_Type<T>& define_iterator(Iterator_Return_T(U::* begin)(), Iterator_Return_T(U::* end)(), Identifier name = "each");

  #include "shared_methods.hpp"

protected:
  //! Bind a Data_Type to a VALUE.
  /*! Throws an exception if the Data_Type is already bound to a
   *  different class.  Any existing instances of the Data_Type will be
   *  bound after this function returns.
   *  \param klass the ruby type to which to bind.
   *  \return *this
   */
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

  static inline VALUE klass_ = Qnil;

  // Typed Data support
  static inline rb_data_type_t* rb_type_ = nullptr;

  typedef std::set<Data_Type<T> *> Instances;

  static Instances & unbound_instances()
  {
    static Instances unbound_instances;
    return unbound_instances;
  }
};

} // namespace Rice

#include "Data_Type.ipp"

#endif // Rice__Data_Type_defn__hpp_