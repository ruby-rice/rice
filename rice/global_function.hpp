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
      Arguments* arguments = 0);

  // FIXME: See Module::define_method with Arg
  template<typename Func_T>
  void define_global_function(
      char const * name,
      Func_T func,
      Arg const& arg);
  

} // Rice

#include "global_function.ipp"

#endif // Rice__global_function__hpp_
