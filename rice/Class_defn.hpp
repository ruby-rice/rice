#ifndef Rice__Class_defn__hpp_
#define Rice__Class_defn__hpp_

#include "Module_defn.hpp"
#include "protect.hpp"

/*!
 *  \example inheritance/animals.cpp
 *  \example callbacks/sample_callbacks.cpp
 */

namespace Rice
{

//! A helper for defining a Class and its methods.
/*! This class provides a C++-style interface to ruby's Class class and
 *  for defining methods on that class.
 */
class Class
  : public Module
{
public:
  //! Default construct a new class wrapper and initialize it to
  //! rb_cObject.
  Class() = default;

  //! Construct a new class wrapper from a ruby object of type T_CLASS.
  Class(VALUE v);

  //! Disallow creation of an instance from Ruby code.
  /*! Undefines the singleton method allocate (or new, if using a
   *  version of ruby prior to 1.7) and the instance method initialize.
   */
  Class & undef_creation_funcs();

  #include "shared_methods.hpp"
};

//! Define a new class in the namespace given by module.
/*! \param module the Module in which to define the class.
 *  \param name the name of the class.
 *  \param superclass the base class to use.
 *  \return the new class.
 */
Class define_class_under(
    Object module,
    char const * name,
    Object superclass = rb_cObject);

//! Define a new class in the default namespace.
/*! \param name the name of the class.
 *  \param superclass the base class to use.
 *  \return the new class.
 */
Class define_class(
    char const * name,
    Object superclass = rb_cObject);

//! Create a new anonymous class.
/*! \param superclass the base class to use.
 *  \return the new class.
 */
Class anonymous_class(
    Object superclass = rb_cObject);

} // namespace Rice

template<>
struct Rice::detail::From_Ruby<Rice::Class>
{
  static Rice::Class convert(VALUE value)
  {
    return Rice::Class(value);
  }
};

#endif // Rice__Class_defn__hpp_

