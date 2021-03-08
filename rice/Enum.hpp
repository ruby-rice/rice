#ifndef Rice__Enum__hpp_
#define Rice__Enum__hpp_

#include "Data_Type.hpp"

namespace Rice
{
  // The C++ struct that we use to store enum information and is wrapped
  // via Ruby Objects
  template<typename Enum_T>
  struct Enum_Storage
  {
    Enum_Storage(std::string name, Enum_T value);
    bool operator==(const Enum_Storage& other);
    int32_t compare(const Enum_Storage& other);

    std::string enumName;
    Enum_T enumValue;
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
template<typename Enum_T>
class Enum
  : public Data_Type<Enum_Storage<Enum_T>>
{
public:
  using Storage_T = Enum_Storage<Enum_T>;
  using Value_T = Data_Object<Storage_T>;

  //! Default constructor.
  Enum() = default;

  //! Construct and initialize.
  Enum(
      char const * name,
      Module module = rb_cObject);

  //! Define a new enum value.
  /*! \param name the name of the enum value.
   *  \param value the value to associate with name.
   *  \return *this
   */
  Enum<Enum_T> & define_value(
      std::string name,
      Enum_T value);

  //! Maps an enum value to the correct Ruby object
  /*! \param klass The bound Ruby class
   *  \param enumValue The enum value
   *  \return Object - The Ruby wrapper */
  static Object from_enum(Class klass, Enum_T enumValue);

private:
  static Object each(Object self);
  static Object to_s(Object self);
  static Object to_i(Object self);
  static Object inspect(Object self);
  static Object compare(Object lhs, Object rhs);
  static Object eql(Object lhs, Object rhs);
  static Object hash(Object self);
  static Object from_int(Class klass, Object i);
};

template<typename T>
Enum<T> define_enum(
    char const * name,
    Module module = rb_cObject);

} // namespace Rice

#include "Enum.ipp"

#endif // Rice__Enum__hpp_