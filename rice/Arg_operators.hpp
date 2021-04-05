#ifndef Rice__Arg_Operators_hpp_
#define Rice__Arg_Operators_hpp_

namespace Rice
{
  //! Build the list of Arg objects into an Arguments object
  /*! Take a list of Arg objects and build up a single Argument
   *  object used later in method dispatch
   */
  [[deprecated("You should no longer enclose multiple Args in parentheses.")]]
  inline MethodInfo* operator,(Arg arg1, Arg arg2)
  {
    MethodInfo* a = new MethodInfo();
    a->add(arg1);
    a->add(arg2);
    return a;
  }

  /*! @see operator,(Arg, Arg)
   */
  [[deprecated("You should no longer enclose multiple Args in parentheses.")]]
  inline MethodInfo* operator,(MethodInfo* methodInfo, Arg arg)
  {
    methodInfo->add(arg);
    return methodInfo;
  }
}
#endif // Rice__Arg_Operators_hpp_