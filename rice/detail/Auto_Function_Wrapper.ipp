#ifndef Rice__detail__Auto_Function_Wrapper__ipp_
#define Rice__detail__Auto_Function_Wrapper__ipp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "method_data.hpp"
#include "../ruby_try_catch.hpp"
#include "../to_from_ruby.hpp"
namespace Rice
{

namespace detail
{

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12, VALUE ruby_arg13, VALUE ruby_arg14, VALUE ruby_arg15)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); Arg13_T arg13(from_ruby<Arg13_T>(ruby_arg13)); Arg14_T arg14(from_ruby<Arg14_T>(ruby_arg14)); Arg15_T arg15(from_ruby<Arg15_T>(ruby_arg15)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12, VALUE ruby_arg13, VALUE ruby_arg14, VALUE ruby_arg15)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); Arg13_T arg13(from_ruby<Arg13_T>(ruby_arg13)); Arg14_T arg14(from_ruby<Arg14_T>(ruby_arg14)); Arg15_T arg15(from_ruby<Arg15_T>(ruby_arg15)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12, VALUE ruby_arg13, VALUE ruby_arg14)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); Arg13_T arg13(from_ruby<Arg13_T>(ruby_arg13)); Arg14_T arg14(from_ruby<Arg14_T>(ruby_arg14)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12, VALUE ruby_arg13, VALUE ruby_arg14)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); Arg13_T arg13(from_ruby<Arg13_T>(ruby_arg13)); Arg14_T arg14(from_ruby<Arg14_T>(ruby_arg14)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12, VALUE ruby_arg13)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); Arg13_T arg13(from_ruby<Arg13_T>(ruby_arg13)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12, VALUE ruby_arg13)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); Arg13_T arg13(from_ruby<Arg13_T>(ruby_arg13)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11, VALUE ruby_arg12)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); Arg12_T arg12(from_ruby<Arg12_T>(ruby_arg12)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10, VALUE ruby_arg11)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); Arg11_T arg11(from_ruby<Arg11_T>(ruby_arg11)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9, VALUE ruby_arg10)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); Arg10_T arg10(from_ruby<Arg10_T>(ruby_arg10)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8, VALUE ruby_arg9)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); Arg9_T arg9(from_ruby<Arg9_T>(ruby_arg9)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7, VALUE ruby_arg8)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); Arg8_T arg8(from_ruby<Arg8_T>(ruby_arg8)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6, VALUE ruby_arg7)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); Arg7_T arg7(from_ruby<Arg7_T>(ruby_arg7)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5, VALUE ruby_arg6)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); Arg6_T arg6(from_ruby<Arg6_T>(ruby_arg6)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4, VALUE ruby_arg5)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); Arg5_T arg5(from_ruby<Arg5_T>(ruby_arg5)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3, VALUE ruby_arg4)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); Arg4_T arg4(from_ruby<Arg4_T>(ruby_arg4)); 
    wrapper->func_(arg0, arg1, arg2, arg3, arg4);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2, VALUE ruby_arg3)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); Arg3_T arg3(from_ruby<Arg3_T>(ruby_arg3)); 
    wrapper->func_(arg0, arg1, arg2, arg3);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); 
    return to_ruby(wrapper->func_(arg0, arg1, arg2));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1, VALUE ruby_arg2)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); Arg2_T arg2(from_ruby<Arg2_T>(ruby_arg2)); 
    wrapper->func_(arg0, arg1, arg2);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); 
    return to_ruby(wrapper->func_(arg0, arg1));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T, typename Arg1_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T>::
call(VALUE ruby_arg0, VALUE ruby_arg1)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); Arg1_T arg1(from_ruby<Arg1_T>(ruby_arg1)); 
    wrapper->func_(arg0, arg1);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Arg0_T>
Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Arg0_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T>::
call(VALUE ruby_arg0)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); 
    return to_ruby(wrapper->func_(arg0));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Arg0_T>
Auto_Function_Wrapper<Func_T, void, Arg0_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Arg0_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T>::
call(VALUE ruby_arg0)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, Arg0_T> *)data.get();
    Arg0_T arg0(from_ruby<Arg0_T>(ruby_arg0)); 
    wrapper->func_(arg0);
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------

} // namespace detail

} // namespace Rice


#endif // Rice__detail__Auto_Function_Wrapper__ipp_

