#ifndef Rice__detail__Auto_Member_Function_Wrapper__ipp_
#define Rice__detail__Auto_Member_Function_Wrapper__ipp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "method_data.hpp"
#include "../ruby_try_catch.hpp"
#include "../to_from_ruby.hpp"
#include <typeinfo>
namespace Rice
{

namespace detail
{

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14, varg15;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;Arg13_T arg13;Arg14_T arg14;Arg15_T arg15;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14, &varg15);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }
		if(args->isOptional(13) && NIL_P(varg13)) { arg13 = args->get(13)->getDefaultValue<Arg13_T>(); } else { arg13 = from_ruby<Arg13_T>(varg13); }
		if(args->isOptional(14) && NIL_P(varg14)) { arg14 = args->get(14)->getDefaultValue<Arg14_T>(); } else { arg14 = from_ruby<Arg14_T>(varg14); }
		if(args->isOptional(15) && NIL_P(varg15)) { arg15 = args->get(15)->getDefaultValue<Arg15_T>(); } else { arg15 = from_ruby<Arg15_T>(varg15); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14, varg15;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;Arg13_T arg13;Arg14_T arg14;Arg15_T arg15;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14, &varg15);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }
		if(args->isOptional(13) && NIL_P(varg13)) { arg13 = args->get(13)->getDefaultValue<Arg13_T>(); } else { arg13 = from_ruby<Arg13_T>(varg13); }
		if(args->isOptional(14) && NIL_P(varg14)) { arg14 = args->get(14)->getDefaultValue<Arg14_T>(); } else { arg14 = from_ruby<Arg14_T>(varg14); }
		if(args->isOptional(15) && NIL_P(varg15)) { arg15 = args->get(15)->getDefaultValue<Arg15_T>(); } else { arg15 = from_ruby<Arg15_T>(varg15); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;Arg13_T arg13;Arg14_T arg14;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }
		if(args->isOptional(13) && NIL_P(varg13)) { arg13 = args->get(13)->getDefaultValue<Arg13_T>(); } else { arg13 = from_ruby<Arg13_T>(varg13); }
		if(args->isOptional(14) && NIL_P(varg14)) { arg14 = args->get(14)->getDefaultValue<Arg14_T>(); } else { arg14 = from_ruby<Arg14_T>(varg14); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13, varg14;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;Arg13_T arg13;Arg14_T arg14;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13, &varg14);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }
		if(args->isOptional(13) && NIL_P(varg13)) { arg13 = args->get(13)->getDefaultValue<Arg13_T>(); } else { arg13 = from_ruby<Arg13_T>(varg13); }
		if(args->isOptional(14) && NIL_P(varg14)) { arg14 = args->get(14)->getDefaultValue<Arg14_T>(); } else { arg14 = from_ruby<Arg14_T>(varg14); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;Arg13_T arg13;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }
		if(args->isOptional(13) && NIL_P(varg13)) { arg13 = args->get(13)->getDefaultValue<Arg13_T>(); } else { arg13 = from_ruby<Arg13_T>(varg13); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12, varg13;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;Arg13_T arg13;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12, &varg13);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }
		if(args->isOptional(13) && NIL_P(varg13)) { arg13 = args->get(13)->getDefaultValue<Arg13_T>(); } else { arg13 = from_ruby<Arg13_T>(varg13); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11, varg12;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;Arg12_T arg12;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11, &varg12);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }
		if(args->isOptional(12) && NIL_P(varg12)) { arg12 = args->get(12)->getDefaultValue<Arg12_T>(); } else { arg12 = from_ruby<Arg12_T>(varg12); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10, varg11;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;Arg11_T arg11;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10, &varg11);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }
		if(args->isOptional(11) && NIL_P(varg11)) { arg11 = args->get(11)->getDefaultValue<Arg11_T>(); } else { arg11 = from_ruby<Arg11_T>(varg11); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9, varg10;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;Arg10_T arg10;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9, &varg10);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }
		if(args->isOptional(10) && NIL_P(varg10)) { arg10 = args->get(10)->getDefaultValue<Arg10_T>(); } else { arg10 = from_ruby<Arg10_T>(varg10); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8, varg9;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;Arg9_T arg9;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8, &varg9);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }
		if(args->isOptional(9) && NIL_P(varg9)) { arg9 = args->get(9)->getDefaultValue<Arg9_T>(); } else { arg9 = from_ruby<Arg9_T>(varg9); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7, varg8;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;Arg8_T arg8;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7, &varg8);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }
		if(args->isOptional(8) && NIL_P(varg8)) { arg8 = args->get(8)->getDefaultValue<Arg8_T>(); } else { arg8 = from_ruby<Arg8_T>(varg8); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6, varg7;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;Arg7_T arg7;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6, &varg7);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }
		if(args->isOptional(7) && NIL_P(varg7)) { arg7 = args->get(7)->getDefaultValue<Arg7_T>(); } else { arg7 = from_ruby<Arg7_T>(varg7); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5, varg6;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;Arg6_T arg6;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5, &varg6);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }
		if(args->isOptional(6) && NIL_P(varg6)) { arg6 = args->get(6)->getDefaultValue<Arg6_T>(); } else { arg6 = from_ruby<Arg6_T>(varg6); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4, varg5;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;Arg5_T arg5;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4, &varg5);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }
		if(args->isOptional(5) && NIL_P(varg5)) { arg5 = args->get(5)->getDefaultValue<Arg5_T>(); } else { arg5 = from_ruby<Arg5_T>(varg5); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4, arg5);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3, arg4));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3, varg4;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;Arg4_T arg4;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3, &varg4);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }
		if(args->isOptional(4) && NIL_P(varg4)) { arg4 = args->get(4)->getDefaultValue<Arg4_T>(); } else { arg4 = from_ruby<Arg4_T>(varg4); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3, arg4);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2, arg3));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2, varg3;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;Arg3_T arg3;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2, &varg3);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }
		if(args->isOptional(3) && NIL_P(varg3)) { arg3 = args->get(3)->getDefaultValue<Arg3_T>(); } else { arg3 = from_ruby<Arg3_T>(varg3); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2, arg3);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1, arg2));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1, varg2;
    Arg0_T arg0;Arg1_T arg1;Arg2_T arg2;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1, &varg2);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }
		if(args->isOptional(2) && NIL_P(varg2)) { arg2 = args->get(2)->getDefaultValue<Arg2_T>(); } else { arg2 = from_ruby<Arg2_T>(varg2); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1, arg2);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1;
    Arg0_T arg0;Arg1_T arg1;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0, arg1));
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

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0, varg1;
    Arg0_T arg0;Arg1_T arg1;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0, &varg1);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }
		if(args->isOptional(1) && NIL_P(varg1)) { arg1 = args->get(1)->getDefaultValue<Arg1_T>(); } else { arg1 = from_ruby<Arg1_T>(varg1); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0, arg1);
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
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0;
    Arg0_T arg0;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(arg0));
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

template<typename Func_T, typename Self_T, typename Arg0_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T, typename Arg0_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE varg0;
    Arg0_T arg0;

    rb_scan_args(argc, argv, args->formatString(Num_Args), 
      &varg0);

    Self_T * obj = from_ruby<Self_T *>(self);
    if(args->isOptional(0) && NIL_P(varg0)) { arg0 = args->get(0)->getDefaultValue<Arg0_T>(); } else { arg0 = from_ruby<Arg0_T>(varg0); }

    Func func = wrapper->func_;
    (*obj.*func)(arg0);
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
template<typename Func_T, typename Ret_T, typename Self_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T> *)data.get();

    rb_scan_args(argc, argv, "0");
    Self_T * obj = from_ruby<Self_T *>(self);

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)());
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

template<typename Func_T, typename Self_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T> *)data.get();
    rb_scan_args(argc, argv, "0");
    Self_T * obj = from_ruby<Self_T *>(self);

    Func func = wrapper->func_;
    (*obj.*func)();
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


#endif // Rice__detail__Auto_Member_Function_Wrapper__ipp_

