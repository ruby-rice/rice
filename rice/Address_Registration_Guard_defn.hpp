#ifndef Rice__Address_Registration_Guard_defn__hpp_
#define Rice__Address_Registration_Guard_defn__hpp_

#include "Object_defn.hpp"
#include "detail/ruby.hpp"
#include "detail/Not_Copyable.hpp"

namespace Rice
{

//! A guard to register a given address with the GC.
/*! Calls rb_gc_register_address upon construction and
 *  rb_gc_unregister_address upon destruction.
 *  For example:
 *  \code
 *    Class Foo
 *    {
 *    public:
 *      Foo()
 *        : string_(rb_str_new2())
 *        , guard_(&string_);
 * 
 *    private:
 *      VALUE string_;
 *      Address_Registration_Guard guard_;
 *    };
 *  \endcode
 */
class Address_Registration_Guard
  : private detail::Not_Copyable
{
public:
  //! Register an address with the GC.
  /*  \param address The address to register with the GC.  The address
   *  must point to a valid ruby object (RObject).
   */
  Address_Registration_Guard(VALUE * address);

  //! Register an Object with the GC.
  /*! \param object The Object to register with the GC.  The object must
   *  not be destroyed before the Address_Registration_Guard is
   *  destroyed.
   */
  Address_Registration_Guard(Object * object);

  //! Unregister an address/Object with the GC.
  /*! Destruct an Address_Registration_Guard.  The address registered
   *  with the Address_Registration_Guard when it was constructed will
   *  be unregistered from the GC.
   */
  ~Address_Registration_Guard();

  //! Get the address that is registered with the GC.
  VALUE * address() const;

  //! Swap with another Address_Registration_Guard.
  void swap(Address_Registration_Guard & other);

  /** Called during Ruby's exit process since we should not call
   * rb_gc unregister_address there
   */
  static void disable();

private:
  static bool enabled;
  static bool exit_handler_registered;

  static void registerExitHandler();

  VALUE * address_;
};

} // namespace Rice

#endif // Rice__Address_Registration_Guard_defn__hpp_
