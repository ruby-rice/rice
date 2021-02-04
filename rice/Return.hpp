#ifndef Rice__Return__hpp_
#define Rice__Return__hpp_

#include <any>

namespace Rice {

  //! Helper for defining Return argument of a method
  /*! This class exposes the ability to define the default values of a
   *  wrapped method. Inspired by how Boost.Python handles keyword and
   *  default arguments, the syntax is simple:
   *
   *  \code
   *    define_method(
   *      "method",
   *      &method,
   *      (Arg("arg1"), Arg("arg2") = 3, Arg("arg3") = true)
   *    );
   *  \endcode
   *
   *  which means "for method &method, it takes 3 arguments
   *  [arg1, arg2, arg3]. Of these arguments, arg2's default is 3
   *  and arg3's default is true.
   *
   *  It may be required to explicitly cast the type of the default
   *  value to prevent compilation errors.
   */

  class Return
  {
  public:
    //! Initialize a new Arg with the name of the argument
    /*! We require the name of the argument because 1) it makes code
     *  easier to read and 2) hopefully Ruby gets keyword arguments
     *  in the future and this means Rice will be ready for it.
     */
    Return(bool takeOwnership = true);
    bool takeOwnership();

  private:
    bool takeOwnership_;
  };
} // Rice

#include "Return.ipp"

#endif // Rice__Return__hpp_
