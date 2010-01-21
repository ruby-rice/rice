#ifndef Rice__detail__wrap_function__hpp_
#define Rice__detail__wrap_function__hpp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "Exception_Handler.hpp"
#include "Wrapped_Function.hpp"
#include "../Object_defn.hpp"
#include "../Data_Object.hpp"
#include "Arguments.hpp"

namespace Rice
{

namespace detail
{

#ifdef DOXYGEN

#else

template<typename Ret_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Arg0_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)() const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

template<typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
Wrapped_Function * wrap_function(
    Ret_T (*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------

#endif // DOXYGEN

} // namespace detail

} // namespace Rice

#include "wrap_function.ipp"

#endif // Rice__detail__wrap_function__hpp_

