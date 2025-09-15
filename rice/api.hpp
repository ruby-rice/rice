#ifndef Rice__Api_hpp_
#define Rice__Api_hpp_

#include "rice_api/ModuleRegistry.hpp"
#include "rice_api/ModuleRegistry.ipp"
#include "rice_api/NativeRegistry.hpp"
#include "rice_api/NativeRegistry.ipp"
#include "rice_api/TypeRegistry.hpp"
#include "rice_api/TypeRegistry.ipp"
#include "rice_api/Registries.hpp"
#include "rice_api/Registries.ipp"
#include "rice_api/Arg.hpp"
#include "rice_api/Arg.ipp"
#include "rice_api/Parameter.hpp"
#include "rice_api/Parameter.ipp"
#include "rice_api/Native.hpp"
#include "rice_api/Native.ipp"

extern "C"
inline void Init_Rice_Api()
{
  Init_Registries();
  Init_Module_Registry();
  Init_Native_Registry();
  Init_Type_Registry();
  Init_Arg();
  Init_Parameter();
  Init_Native();
}
#endif
