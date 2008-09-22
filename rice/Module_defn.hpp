#ifndef Rice__Module_defn__hpp_
#define Rice__Module_defn__hpp_

#include "Object_defn.hpp"
#include "Module_impl.hpp"
#include "to_from_ruby_defn.hpp"
#include <memory>

namespace Rice
{

class Array;
class Class;
class String;

//! A helper for defining a Module and its methods.
/*! This class provides a C++-style interface to ruby's Module class and
 *  for defining methods on that module.
 *
 *  Many of the methods are defined in Module_impl.hpp so that they can
 *  return a reference to the most derived type.
 */
class Module
  // TODO: we can't inherit from Builtin_Object, because Class needs
  // type T_CLASS and Module needs type T_MODULE
  : public Module_impl<Module_base, Module>
{
public:
  //! Default construct a Module and initialize it to rb_cObject.
  Module();

  //! Construct a Module from an existing Module object.
  Module(VALUE v);

  //! Return the name of the module.
  String name() const;

  //! Swap with another Module.
  void swap(Module & other);

  //! Return an array containing the Module's ancestors.
  /*! You will need to include Array.hpp to use this function.
   */
  Array ancestors() const;

  //! Return the module's singleton class.
  /*! You will need to include Class.hpp to use this function.
   */
  Class singleton_class() const;
};

//! Define a new module in the namespace given by module.
/*! \param module the module in which to define the new module.
 *  \param name the name of the new module.
 */
Module define_module_under(
    Object module,
    char const * name);

//! Define a new module in the default namespace.
/*! \param name the name of the new module.
 */
Module define_module(
    char const * name);

//! Create a new anonymous module.
/*! \return the new module.
 */
Module anonymous_module();

} // namespace Rice

template<>
inline
Rice::Module from_ruby<Rice::Module>(Rice::Object x)
{
  return Rice::Module(x);
}

template<>
inline
Rice::Object to_ruby<Rice::Module>(Rice::Module const & x)
{
  return x;
}

#endif // Rice__Module_defn__hpp_

