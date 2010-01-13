// This is a generated file.  DO NOT EDIT!!
#ifdef DOXYGEN

//! Call the Ruby method specified by 'id' on object 'obj'.
/*! Pass in arguments (arg1, arg2, ...).  The arguments will be converted to
 *  Ruby objects with to_ruby<>.  The return value will automatically be
 *  converted to type Retval_T with from_ruby<>.
 *
 *  E.g.:
 *  \code
 *    float y = x.call<float>("foo", z, 42);
 *  \endcode
 */
template<typename Retval_T>
Retval_T call(Identifier id, T1 arg1, T2 arg2, ...) const;

//! Version of call which defaults to Object return type.
Object call(Identifier id, T1 arg1, T2 arg2, ...) const;
#else

Object call(Identifier id) const;

template<typename T0>
Object call(Identifier id, T0 arg0) const;

template<typename T0, typename T1>
Object call(Identifier id, T0 arg0, T1 arg1) const;

template<typename T0, typename T1, typename T2>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2) const;

template<typename T0, typename T1, typename T2, typename T3>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13) const;

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
Object call(Identifier id, T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13, T14 arg14) const;

#endif
