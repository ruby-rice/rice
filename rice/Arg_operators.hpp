#ifndef Rice__Arg_Operators_hpp_
#define Rice__Arg_Operators_hpp_

namespace Rice
{

  //! Build the list of Arg objects into an Arguments object
  /*! Take a list of Arg objects and build up a single Argument
   *  object used later in method dispatch
   */
  Arguments* operator,(Arg arg1, Arg arg2);

  /*! @see operator,(Arg, Arg)
   */
  Arguments* operator,(Arguments* arguments, Arg arg);

}

#endif // Rice__Arg_Operators_hpp_
