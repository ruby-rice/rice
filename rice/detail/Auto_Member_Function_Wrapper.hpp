#ifndef Rice__detail__Auto_Member_Function_Wrapper__hpp_
#define Rice__detail__Auto_Member_Function_Wrapper__hpp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

namespace Rice
{

namespace detail
{

#ifdef DOXYGEN

#else

template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T = void, typename Arg1_T = void, typename Arg2_T = void, typename Arg3_T = void, typename Arg4_T = void, typename Arg5_T = void, typename Arg6_T = void, typename Arg7_T = void, typename Arg8_T = void, typename Arg9_T = void, typename Arg10_T = void, typename Arg11_T = void, typename Arg12_T = void, typename Arg13_T = void, typename Arg14_T = void, typename Arg15_T = void>
class Auto_Member_Function_Wrapper
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 16;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T, typename Arg15_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T, Arg15_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 16;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 15;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T, typename Arg14_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T, Arg14_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 15;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 14;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T, typename Arg13_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T, Arg13_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 14;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 13;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T, typename Arg12_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T, Arg12_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 13;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 12;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T, typename Arg11_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T, Arg11_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 12;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 11;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T, typename Arg10_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T, Arg10_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 11;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 10;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T, typename Arg9_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T, Arg9_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 10;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 9;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T, typename Arg8_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T, Arg8_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 9;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 8;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T, typename Arg7_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T, Arg7_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 8;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 7;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T, typename Arg6_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T, Arg6_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 7;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 6;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T, typename Arg5_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T, Arg5_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 6;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 5;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T, typename Arg4_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T, Arg4_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 5;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 4;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T, typename Arg3_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T, Arg3_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 4;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T, Arg2_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 3;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T, typename Arg2_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T, Arg2_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 3;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T, typename Arg1_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T, Arg1_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 2;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T, typename Arg1_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T, Arg1_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 2;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T, typename Arg0_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T, Arg0_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 1;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T, typename Arg0_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T, Arg0_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 1;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
template<typename Func_T, typename Ret_T, typename Self_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 0;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 0;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
#endif // DOXYGEN

} // namespace detail

} // namespace Rice

#include "Auto_Member_Function_Wrapper.ipp"

#endif // Rice__detail__Auto_Member_Function_Wrapper__hpp_

