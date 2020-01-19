#ifndef Rice__Object_defn__hpp_
#define Rice__Object_defn__hpp_

/*! \file Object.hpp
 */

#include "Identifier.hpp"
#include "detail/ruby.hpp"

#include <iosfwd>
#include <vector>

namespace Rice
{

class Class;
class String;
class Array;

//! The base class for all Objects
/*! Perhaps the name "Object" is a misnomer, because this class really
 *  holds an object reference, not an object.
 */
class Object
{
public:
  //! Encapsulate an existing ruby object.
  Object(VALUE value = Qnil) : value_(value) { }

  //! Copy constructor
  Object(Object const & other) : value_(other.value()) { }

  //! Destructor
  virtual ~Object() { }

  //! Returns false if the object is nil or false; returns true
  //! otherwise.
  // Having this conversion also prevents accidental conversion to
  // undesired integral types (e.g. long or int) by making the
  // conversion ambiguous.
  bool test() const { return RTEST(value_); }

  //! Returns false if the object is nil or false; returns true
  //! otherwise.
  operator bool() const { return test(); }

  //! Returns true if the object is nil, false otherwise.
  bool is_nil() const { return NIL_P(value_); }

  //! Implicit conversion to VALUE.
  operator VALUE() const { return value_; }

  //! Explicitly get the encapsulated VALUE.
  // Returns a const ref so that Address_Registration_Guard can access
  // the address where the VALUE is stored
  VALUE const volatile & value() const { return value_; }

  //! Get the class of an object.
  /*! \return the object's Class.
   */
  Class class_of() const;

  //! Compare this object to another object.
  /*! Gets the result of self <=> other and returns the result.  The
   *  result will be less than zero if self < other, greater than zero
   *  if self > other, and equal to zero if self == other.
   */
  int compare(Object const & other) const;

  //! Return a string representation of an object.
  /*! \return the result of calling to_s on the object.  A String is not
   *  returned, because it is not possible to return an instance of a
   *  derived class.
   */
  String to_s() const;

  //! Inspect the object.
  /*! \return the result of calling inspect on the object.  A String is
   *  not returned, because it is not possible to return an instance of
   *  a derived class.
   */
  String inspect() const;

  //! Freeze the object.
  void freeze();

  //! Determine if the object is frozen.
  /*! \return true if the object is frozen, false otherwise.
   */
  bool is_frozen() const;

  //! Swap with another Object.
  void swap(Object & other);

  //! Evaluate the given string in the context of the object.
  /*! This is equivalant to calling obj.instance_eval(s) from inside the
   *  interpreter.
   *  \return the result of the expression.
   */
  Object instance_eval(String const & s);

  //! Return the type of the underlying C object.
  /*! This is equivalent to calling rb_type(obj).
   * \return the type of the underlying C object (e.g. T_DATA, T_ARRAY,
   * etc.).
   */
  int rb_type() const;

  //! Determine whether the object is an instance of a class/module.
  /*! \param klass a class or module.
   *  \return true if the object is an instance of the given
   *  class/module or one of its descendants.
   */
  bool is_a(Object klass) const;

  //! Determine if the objects responds to a method.
  /*! \param id the name of the method
   *  \return true if the objects responds to the method, false
   *  otherwise.
   */
  bool respond_to(Identifier id) const;

  //! Determine whether class is the object's class.
  /*! \param klass a class.
   *  \return true if the object is an instance of the given class.
   */
  bool is_instance_of(Object klass) const;

  //! Set an instance variable.
  /*! \param name the name of the instance variable to set (including
   *  the leading @ sign)
   *  \param value the value of the variable, which will be converted to
   *  a Ruby type if necessary.
   */
  template<typename T>
  void iv_set(
      Identifier name,
      T const & value);

  //! Get the value of an instance variable.
  /*! \param name the name of the instance variable to get
   *  \return the value of the instance variable
   */
  Object iv_get(
      Identifier name) const;

  //! Get the value of an instance variable, but don't warn if it is
  //unset.
  /*! \param name the name of the instance variable to get
   *  \return the value of the instance variable
   */
  Object attr_get(
      Identifier name) const;

  //! Call the Ruby method specified by 'id' on object 'obj'.
  /*! Pass in arguments (arg1, arg2, ...).  The arguments will be converted to
  *  Ruby objects with to_ruby<>.
  *
  *  E.g.:
  *  \code
  *    Rice::Object obj = x.call("foo", "one", 2);
  *  \endcode
  *
  *  If a return type is specified, the return value will automatically be
  *  converted to that type as long as 'from_ruby' exists for that type.
  *
  *  E.g.:
  *  \code
  *    float ret = x.call<float>("foo", z, 42);
  *  \endcode
  */
  template<typename ...ArgT>
  Object call(Identifier id, ArgT... args) const;

  //! Vectorized call.
  /*! Calls the method identified by id with the list of arguments
   *  identified by args.
   *  \param id the name of the method to call
   *  \param args the arguments to the method
   *  \return the return value of the method call
   */
  Object vcall(Identifier id, Array args);

  //! Mark the object with the garbage collector.
  void mark() const;

protected:
  //! Set the encapsulated value.
  void set_value(VALUE v);

  //! Unpack the provided arguments and convert them all to Ruby types.
  template<typename ...ArgT>
  std::vector<VALUE> convert_args(ArgT&... args) const;

private:
  volatile VALUE value_;
};

std::ostream & operator<<(std::ostream & out, Object const & obj);

bool operator==(Object const & lhs, Object const & rhs);
bool operator!=(Object const & lhs, Object const & rhs);
bool operator<(Object const & lhs, Object const & rhs);
bool operator>(Object const & lhs, Object const & rhs);

extern Object const Nil;
extern Object const True;
extern Object const False;
extern Object const Undef;

} // namespace Rice

#endif // Rice__Object_defn__hpp_

