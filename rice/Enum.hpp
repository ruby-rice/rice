#ifndef Rice__Enum__hpp_
#define Rice__Enum__hpp_

#include "to_from_ruby_defn.hpp"
#include "Address_Registration_Guard.hpp"
#include "Array.hpp"
#include "Hash.hpp"
#include "String.hpp"
#include "Module.hpp"
#include "Data_Type.hpp"

namespace Rice
{

//! Default traits for the Enum class template.
template<typename Enum_T>
struct Default_Enum_Traits
{
  //! Converts the enum value to a long.
  static long as_long(Enum_T value);
};

/*!
 *  \example enum/sample_enum.cpp
 */

//! A wrapper for enumerated types.
/*! Provides a simple type-safe wrapper for enumerated types.  At the
 *  ruby level, the class will have convenience methods for iterating
 *  over all the defined enum values, converting the values to strings,
 *  and more.
 *
 *  \param Enum_T the enumerated type
 *  \param Enum_Traits specifies the traits of the enumerated type.
 *
 *  Example:
 *  \code
 *    enum Color { Red, Green, Blue };
 *    Enum<Color> rb_cColor = define_enum<Color>("Color")
 *      .define_value("Red", Red)
 *      .define_value("Green", Green)
 *      .define_value("Blue", Blue);
 *  \endcode
 */
template<typename Enum_T, typename Enum_Traits = Default_Enum_Traits<Enum_T> >
class Enum
  : public Module_impl<Data_Type<Enum_T>, Enum<Enum_T, Enum_Traits> >
{
public:
  //! Default constructor.
  Enum();

  //! Construct and initialize.
  Enum(
      char const * name,
      Module module = rb_cObject);

  //! Copy constructor.
  Enum(Enum const & other);

  //! Assignment operator.
  Enum & operator=(Enum const & other);

  //! Destructor.
  virtual ~Enum();

  //! Define a new enum value.
  /*! \param name the name of the enum value.
   *  \param value the value to associate with name.
   *  \return *this
   */
  Enum<Enum_T, Enum_Traits> & define_value(
      char const * name,
      Enum_T value);

  void swap(Enum & other);

private:
  //! Initialize the enum type.
  /*! Must be called only once.
   *  \param name the name of the class to define
   *  \param module the module in which to place the enum class.
   *  \return *this
   */
  Enum<Enum_T, Enum_Traits> & initialize(
      char const * name,
      Module module = rb_cObject);

private:
  static Object each(Object self);
  static Object to_s(Object self);
  static Object to_i(Object self);
  static Object inspect(Object self);
  static Object compare(Object lhs, Object rhs);
  static Object eql(Object lhs, Object rhs);
  static Object hash(Object self);
  static Object from_int(Class klass, Object i);

private:
  Array enums_;
  Address_Registration_Guard enums_guard_;

  Hash names_;
  Address_Registration_Guard names_guard_;
};

template<typename T>
Enum<T> define_enum(
    char const * name,
    Module module = rb_cObject);

} // namespace Rice

#include "Enum.ipp"

#endif // Rice__Enum__hpp_

