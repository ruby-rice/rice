#ifndef Rice__Class__hpp_
#define Rice__Class__hpp_

namespace Rice
{
  //! A helper for defining a Class and its methods.
  /*! This class provides a C++-style interface to ruby's Class class and
   *  for defining methods on that class.
   */
  class Class: public Module
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

    // Create a new instance
    template<typename ...Arg_Ts>
    Object create(Arg_Ts ...args);

    //! Class name
    /*! \return std::string.
     */
    const std::string name() const;

    //! Base class name - does not include any parent modules
    /*! \return std::string.
     */
    const std::string base_name() const;

#include "shared_methods.hpp"
  };

  //! Define a new class in the namespace given by module.
  /*! \param module the Module in which to define the class.
   *  \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class_under(Object parent, char const * name, const Class& superclass = rb_cObject);
  Class define_class_under(Object parent, Identifier id, const Class& superclass = rb_cObject);

  //! Define a new class in the default namespace.
  /*! \param name the name of the class.
   *  \param superclass the base class to use.
   *  \return the new class.
   */
  Class define_class(char const * name, const Class& superclass = rb_cObject);

  //! Create a new anonymous class.
  /*! \return the new class.
   */
  Class anonymous_class();
} // namespace Rice

#endif // Rice__Class__hpp_