#ifndef Rice__String__hpp_
#define Rice__String__hpp_

#include "Identifier.hpp"
#include "Builtin_Object_defn.hpp"
#include "to_from_ruby_defn.hpp"
#include "detail/ruby.hpp"
#include <string>

namespace Rice
{

//! A Wraper for the ruby String class.
/*! This class provides a C++-style interface to ruby's String class and
 *  its associated rb_str_* functions.
 *
 *  Example:
 *  \code
 *    String s(String::format("%s: %d", "foo", 42));
 *    std::cout << s.length() << std::endl;
 *  \endcode
 */
class String
  : public Builtin_Object<RString, T_STRING>
{
public:
  //! Construct a new string.
  String();

  //! Wrap an existing string.
  String(VALUE v);

  //! Wrap an existing string.
  String(Object v);

  //! Construct a String from an Identifier.
  String(Identifier id);

  //! Construct a String from a null-terminated C string.
  String(char const * s);

  //! Construct a String from an std::string.
  String(std::string const & s);

  //! Format a string using printf-style formatting.
  static String format(char const * s, ...);

  //! Get the length of the String.
  /*! \return the length of the string.
   */
  size_t length() const;

  //! Get the character at the given index.
  /*! \param index the desired index.
   *  \return the character at the given index.
   */
  char operator[](ptrdiff_t index) const;

  //! Return a pointer to the beginning of the underlying C string.
  char const * c_str() const;

  //! Return a copy of the string as an std::string.
  std::string str() const;

  //! Create an Identifier from the String.
  /*! Calls rb_intern to create an ID.
   *  \return an Identifier holding the ID returned from rb_intern.
   */
  Identifier intern() const;
};

} // namespace Rice

template<>
inline
Rice::String from_ruby<Rice::String>(Rice::Object x)
{
  return Rice::String(x);
}

template<>
inline
Rice::Object to_ruby<Rice::String>(Rice::String const & x)
{
  return x;
}

#endif // Rice__String__hpp_

