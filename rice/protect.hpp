#ifndef Rice__protect__hpp_
#define Rice__protect__hpp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "Object_defn.hpp"
namespace Rice
{

#ifdef DOXYGEN
/*! \file
 *  \brief A collection of functions (overloaded on number of
 *  arguments) for calling C functions that might raise Ruby exceptions.
 */

//! Call the C function f with arguments (arg1, arg2, ...).
/*! E.g.:
 *  \code
 *    VALUE x = protect(rb_ary_new);
 *    protect(rb_ary_push, x, INT2NUM(42));
 *  \endcode
 *
 *  Note that this function makes copies of all of its arguments; it
 *  does not take anything by reference.  All of the copies are const so
 *  that protect will not work if f takes a non-const
 *  reference to any of its arguments (though you can use non-const
 *  pointers).
 */
VALUE protect(Function f, T1 arg1, T2 arg2, ...);
#else

template<typename Fun>
VALUE protect(Fun fun);

template<typename Fun, typename T0>
VALUE protect(Fun fun, T0 const & t0);

template<typename Fun, typename T0, typename T1>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1);

template<typename Fun, typename T0, typename T1, typename T2>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2);

template<typename Fun, typename T0, typename T1, typename T2, typename T3>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13);

template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14);

#endif // DOXYGEN

} // namespace Rice

#include "protect.ipp"

#endif // Rice__protect__hpp_

