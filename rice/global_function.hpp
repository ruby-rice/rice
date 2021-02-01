#ifndef Rice__global_function__hpp_
#define Rice__global_function__hpp_

#include "Arg.hpp"

namespace Rice
{

  /**
   * Helper forwarder method to easily wrap
   * globally available functions. This simply
   * forwards off a call to define_module_function
   * on rb_mKernel
   */
  template<typename Func_T>
  void define_global_function(
      char const * name,
      Func_T func,
      Arguments* arguments);

  // FIXME: See Module::define_method with Arg
  template<typename Func_T, typename...Arg_Ts>
  void define_global_function(
      char const * name,
      Func_T func,
      Arg_Ts const& ...args);
  
} // Rice

#include "global_function.ipp"

#endif // Rice__global_function__hpp_
