#ifndef Rice__Arg_Operators_hpp_
#define Rice__Arg_Operators_hpp_

namespace Rice 
{

  /**
   * Comma operators to take a list of Arg objects
   * and build up a single Argument object used
   * later in method dispatch
   */
  Arguments& operator,(const Arg& arg1, const Arg& arg2)
  {
    Arguments* a = new Arguments();
    a->add(arg1);
    a->add(arg2);
    return *a;
  }

  Arguments& operator,(Arguments& arguments, const Arg& arg)
  {
    arguments.add(arg);
    return arguments;
  }

}

#endif // Rice__Arg_Operators_hpp_
