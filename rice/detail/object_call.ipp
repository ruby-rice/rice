// This is a generated file.  DO NOT EDIT!!
#include "../protect.hpp"
#include "../to_from_ruby.hpp"

inline Rice::Object Rice::Object::
call(Identifier id) const
{
  VALUE args[] = {  };
  return protect(rb_funcall2, value(), id, 0, args);
}

template<typename T0>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0) const
{
  VALUE args[] = { to_ruby(arg0) };
  return protect(rb_funcall2, value(), id, 1, args);
}

template<typename T0, typename T1>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1) };
  return protect(rb_funcall2, value(), id, 2, args);
}

template<typename T0, typename T1, typename T2>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2) };
  return protect(rb_funcall2, value(), id, 3, args);
}

template<typename T0, typename T1, typename T2, typename T3>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3) };
  return protect(rb_funcall2, value(), id, 4, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4) };
  return protect(rb_funcall2, value(), id, 5, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5) };
  return protect(rb_funcall2, value(), id, 6, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6) };
  return protect(rb_funcall2, value(), id, 7, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7) };
  return protect(rb_funcall2, value(), id, 8, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8) };
  return protect(rb_funcall2, value(), id, 9, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8), to_ruby(arg9) };
  return protect(rb_funcall2, value(), id, 10, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8), to_ruby(arg9), to_ruby(arg10) };
  return protect(rb_funcall2, value(), id, 11, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8), to_ruby(arg9), to_ruby(arg10), to_ruby(arg11) };
  return protect(rb_funcall2, value(), id, 12, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8), to_ruby(arg9), to_ruby(arg10), to_ruby(arg11), to_ruby(arg12) };
  return protect(rb_funcall2, value(), id, 13, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8), to_ruby(arg9), to_ruby(arg10), to_ruby(arg11), to_ruby(arg12), to_ruby(arg13) };
  return protect(rb_funcall2, value(), id, 14, args);
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline Rice::Object Rice::Object::
call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13, T14 arg14) const
{
  VALUE args[] = { to_ruby(arg0), to_ruby(arg1), to_ruby(arg2), to_ruby(arg3), to_ruby(arg4), to_ruby(arg5), to_ruby(arg6), to_ruby(arg7), to_ruby(arg8), to_ruby(arg9), to_ruby(arg10), to_ruby(arg11), to_ruby(arg12), to_ruby(arg13), to_ruby(arg14) };
  return protect(rb_funcall2, value(), id, 15, args);
}

