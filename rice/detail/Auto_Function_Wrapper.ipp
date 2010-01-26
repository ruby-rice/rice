#ifndef Rice__detail__Auto_Function_Wrapper__ipp_
#define Rice__detail__Auto_Function_Wrapper__ipp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "method_data.hpp"
#include "../ruby_try_catch.hpp"
#include "../to_from_ruby.hpp"
#include "traits.hpp"
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14, varg15;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14, &varg15);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(12, varg12);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(13, varg13);
      typedef typename sanitize< Arg15_T >::Type Arg15_Type;
        Arg15_Type arg15 = args->getArgumentOrDefault<Arg15_Type>(14, varg14);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14, &varg15);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(13, varg13);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(14, varg14);
      typedef typename sanitize< Arg15_T >::Type Arg15_Type;
        Arg15_Type arg15 = args->getArgumentOrDefault<Arg15_Type>(15, varg15);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14, varg15;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14, &varg15);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(12, varg12);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(13, varg13);
      typedef typename sanitize< Arg15_T >::Type Arg15_Type;
        Arg15_Type arg15 = args->getArgumentOrDefault<Arg15_Type>(14, varg14);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14, &varg15);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(13, varg13);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(14, varg14);
      typedef typename sanitize< Arg15_T >::Type Arg15_Type;
        Arg15_Type arg15 = args->getArgumentOrDefault<Arg15_Type>(15, varg15);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(12, varg12);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(13, varg13);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(13, varg13);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(14, varg14);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(12, varg12);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(13, varg13);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(13, varg13);
      typedef typename sanitize< Arg14_T >::Type Arg14_Type;
        Arg14_Type arg14 = args->getArgumentOrDefault<Arg14_Type>(14, varg14);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(12, varg12);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(13, varg13);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(12, varg12);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      typedef typename sanitize< Arg13_T >::Type Arg13_Type;
        Arg13_Type arg13 = args->getArgumentOrDefault<Arg13_Type>(13, varg13);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(11, varg11);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      typedef typename sanitize< Arg12_T >::Type Arg12_Type;
        Arg12_Type arg12 = args->getArgumentOrDefault<Arg12_Type>(12, varg12);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(10, varg10);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      typedef typename sanitize< Arg11_T >::Type Arg11_Type;
        Arg11_Type arg11 = args->getArgumentOrDefault<Arg11_Type>(11, varg11);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(9, varg9);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      typedef typename sanitize< Arg10_T >::Type Arg10_Type;
        Arg10_Type arg10 = args->getArgumentOrDefault<Arg10_Type>(10, varg10);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(8, varg8);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      typedef typename sanitize< Arg9_T >::Type Arg9_Type;
        Arg9_Type arg9 = args->getArgumentOrDefault<Arg9_Type>(9, varg9);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(7, varg7);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      typedef typename sanitize< Arg8_T >::Type Arg8_Type;
        Arg8_Type arg8 = args->getArgumentOrDefault<Arg8_Type>(8, varg8);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(6, varg6);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      typedef typename sanitize< Arg7_T >::Type Arg7_Type;
        Arg7_Type arg7 = args->getArgumentOrDefault<Arg7_Type>(7, varg7);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(5, varg5);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      typedef typename sanitize< Arg6_T >::Type Arg6_Type;
        Arg6_Type arg6 = args->getArgumentOrDefault<Arg6_Type>(6, varg6);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4, varg5;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(4, varg4);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4, &varg5);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      typedef typename sanitize< Arg5_T >::Type Arg5_Type;
        Arg5_Type arg5 = args->getArgumentOrDefault<Arg5_Type>(5, varg5);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4, arg5);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3, arg4));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3, varg4;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(3, varg3);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3, &varg4);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      typedef typename sanitize< Arg4_T >::Type Arg4_Type;
        Arg4_Type arg4 = args->getArgumentOrDefault<Arg4_Type>(4, varg4);
      wrapper->func_(arg0, arg1, arg2, arg3, arg4);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2, &varg3);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      return to_ruby(wrapper->func_(arg0, arg1, arg2, arg3));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T, Arg3_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2, varg3;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2, &varg3);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(2, varg2);
      wrapper->func_(arg0, arg1, arg2, arg3);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2, &varg3);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      typedef typename sanitize< Arg3_T >::Type Arg3_Type;
        Arg3_Type arg3 = args->getArgumentOrDefault<Arg3_Type>(3, varg3);
      wrapper->func_(arg0, arg1, arg2, arg3);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T, Arg2_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1, &varg2);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      return to_ruby(wrapper->func_(arg0, arg1, arg2));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      return to_ruby(wrapper->func_(arg0, arg1, arg2));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T, Arg2_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1, varg2;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1, &varg2);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(1, varg1);
      wrapper->func_(arg0, arg1, arg2);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1, &varg2);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      typedef typename sanitize< Arg2_T >::Type Arg2_Type;
        Arg2_Type arg2 = args->getArgumentOrDefault<Arg2_Type>(2, varg2);
      wrapper->func_(arg0, arg1, arg2);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T, typename Arg1_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T, Arg1_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0, &varg1);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      return to_ruby(wrapper->func_(arg0, arg1));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      return to_ruby(wrapper->func_(arg0, arg1));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T, typename Arg1_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T, Arg1_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0, varg1;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0, &varg1);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(0, varg0);
      wrapper->func_(arg0, arg1);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0, &varg1);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      typedef typename sanitize< Arg1_T >::Type Arg1_Type;
        Arg1_Type arg1 = args->getArgumentOrDefault<Arg1_Type>(1, varg1);
      wrapper->func_(arg0, arg1);
      return Qnil;
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Arg0_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, Arg0_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        , &varg0);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      return to_ruby(wrapper->func_(arg0));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      return to_ruby(wrapper->func_(arg0));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Arg0_T>
VALUE Auto_Function_Wrapper<Func_T, void, Arg0_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, Arg0_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void, Arg0_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE varg0;

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        , &varg0);

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 

      wrapper->func_(arg0);
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        , &varg0);

      typedef typename sanitize< Arg0_T >::Type Arg0_Type;
        Arg0_Type arg0 = args->getArgumentOrDefault<Arg0_Type>(0, varg0);
      wrapper->func_(arg0);
      return Qnil;
    }
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
template<typename Func_T, typename Ret_T>
Auto_Function_Wrapper<Func_T, Ret_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler ? handler : new Default_Exception_Handler)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T>::
call()
{
  Auto_Function_Wrapper<Func_T, Ret_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T>*)data.get();

    return to_ruby(wrapper->func_());
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

template<typename Func_T>
Auto_Function_Wrapper<Func_T, void>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler ? handler : new Default_Exception_Handler)
  , arguments_(arguments)
{
}

template<typename Func_T>
VALUE Auto_Function_Wrapper<Func_T, void>::
call()
{
  Auto_Function_Wrapper<Func_T, void> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, void> *)data.get();

    wrapper->func_();
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

} // namespace detail

} // namespace Rice


#endif // Rice__detail__Auto_Function_Wrapper__ipp_

