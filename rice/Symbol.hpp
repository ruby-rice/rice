#ifndef Rice__Symbol__hpp_
#define Rice__Symbol__hpp_

#include "Identifier.hpp"
#include "Object.hpp"
#include "detail/ruby.hpp"
#include <string>

namespace Rice
{

//! A wrapper for ruby's Symbol class.
/*! Symbols are internal identifiers in ruby.  They are singletons and
 *  can be thought of as frozen strings.  They differ from an Identifier
 *  in that they are in fact real Objects, but they can be converted
 *  back and forth between Identifier and Symbol.
 */
class Symbol
  : public Object
{
public:
  //! Wrap an existing symbol.
  Symbol(VALUE v);

  //! Wrap an existing symbol.
  Symbol(Object v);

  //! Construct a Symbol from an Identifier.
  Symbol(Identifier id);

  //! Construct a Symbol from a null-terminated C string.
  Symbol(char const * s = "");

  //! Construct a Symbol from an std::string.
  Symbol(std::string const & s);

  //! Return a string representation of the Symbol.
  char const * c_str() const;

  //! Return a string representation of the Symbol.
  std::string str() const;

  //! Return the Symbol as an Identifier.
  Identifier to_id() const;
};

} // namespace Rice

template<>
inline
Rice::Symbol from_ruby<Rice::Symbol>(Rice::Object x)
{
  return Rice::Symbol(x);
}

template<>
inline
Rice::Object to_ruby<Rice::Symbol>(Rice::Symbol const & x)
{
  return x;
}

#include "Symbol.ipp"

#endif // Rice__Symbol__hpp_

