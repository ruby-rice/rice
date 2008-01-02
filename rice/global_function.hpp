#ifndef Rice__global_function__hpp_
#define Rice__global_function__hpp_

namespace Rice
{

template<typename Func_T>
void define_global_function(
    char const * name,
    Func_T func);

} // Rice

#include "global_function.ipp"

#endif // Rice__global_function__hpp_
