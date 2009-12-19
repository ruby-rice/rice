#include "Arg_impl.hpp"
#include "detail/Arguments.hpp"
#include "Arg_operators.hpp"

namespace Rice {

  Arguments* operator,(Arg arg1, Arg arg2)
  {
    Arguments* a = new Arguments();
    a->add(arg1);
    a->add(arg2);
    return a;
  }

  Arguments* operator,(Arguments* arguments, Arg arg)
  {
    arguments->add(arg);
    return arguments;
  }

}
