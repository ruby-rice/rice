#ifndef Rice__Exception_Base_defn__hpp_
#define Rice__Exception_Base_defn__hpp_

#include "Object_defn.hpp"

namespace Rice
{

//! An abstract interface for Exception
/*! This class exists to prevent a circular reference between
 *  Exception.hpp and ruby_try_catch.hpp
 */
class Exception_Base
  : public std::exception
  , public Object
{
public:
  Exception_Base(VALUE v);

  virtual ~Exception_Base() throw() = 0;

  virtual char const * what() const throw() = 0;
};

} // Rice

#endif // Rice__Exception_Base_defn__hpp_
