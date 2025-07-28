#ifndef Rice__Ruby_Api_hpp_
#define Rice__Ruby_Api_hpp_

#include "ruby_api/NativeRegistry.hpp"
#include "ruby_api/NativeRegistry.ipp"
#include "ruby_api/TypeRegistry.hpp"
#include "ruby_api/TypeRegistry.ipp"
#include "ruby_api/Registries.hpp"
#include "ruby_api/Registries.ipp"
#include "ruby_api/Arg.hpp"
#include "ruby_api/Arg.ipp"
#include "ruby_api/Parameter.hpp"
#include "ruby_api/Parameter.ipp"
#include "ruby_api/Native.hpp"
#include "ruby_api/Native.ipp"

extern "C"
inline void Init_Ruby_Api()
{
  Init_Registries();
  Init_Native_Registry();
  Init_Type_Registry();
  Init_Arg();
  Init_Parameter();
  Init_Native();
}

#endif
