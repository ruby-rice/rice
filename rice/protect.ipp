#ifndef Rice__protect__ipp_
#define Rice__protect__ipp_

// This is a generated file.  DO NOT EDIT!!


// This causes problems with certain C++ libraries
#undef TYPE

#include "detail/protect.hpp"
namespace Rice
{

namespace detail
{

template<typename Fun>
class Ruby_Function_0
{
public:
  Ruby_Function_0(Fun f);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_0 * f);
private:
  Fun f_;
  
};

template<typename Fun>
inline Ruby_Function_0<Fun>::
Ruby_Function_0(Fun f)
  : f_(f)
{ }

template<typename Fun>
inline VALUE Ruby_Function_0<Fun>::
operator()()
{
  return f_();
}

template<typename Fun>
inline VALUE Ruby_Function_0<Fun>::
call(Ruby_Function_0 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun>
inline VALUE protect(Fun fun)
{
  typedef detail::Ruby_Function_0<Fun> RF;
  RF f(fun);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1>
class Ruby_Function_1
{
public:
  Ruby_Function_1(Fun f, T1 const & t1);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_1 * f);
private:
  Fun f_;
  T1 const & t1_; 
};

template<typename Fun, typename T1>
inline Ruby_Function_1<Fun, T1>::
Ruby_Function_1(Fun f, T1 const & t1)
  : f_(f), t1_(t1)
{ }

template<typename Fun, typename T1>
inline VALUE Ruby_Function_1<Fun, T1>::
operator()()
{
  return f_(t1_);
}

template<typename Fun, typename T1>
inline VALUE Ruby_Function_1<Fun, T1>::
call(Ruby_Function_1 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1>
inline VALUE protect(Fun fun, T1 const & t1)
{
  typedef detail::Ruby_Function_1<Fun, T1> RF;
  RF f(fun, t1);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2>
class Ruby_Function_2
{
public:
  Ruby_Function_2(Fun f, T1 const & t1, T2 const & t2);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_2 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; 
};

template<typename Fun, typename T1, typename T2>
inline Ruby_Function_2<Fun, T1, T2>::
Ruby_Function_2(Fun f, T1 const & t1, T2 const & t2)
  : f_(f), t1_(t1), t2_(t2)
{ }

template<typename Fun, typename T1, typename T2>
inline VALUE Ruby_Function_2<Fun, T1, T2>::
operator()()
{
  return f_(t1_, t2_);
}

template<typename Fun, typename T1, typename T2>
inline VALUE Ruby_Function_2<Fun, T1, T2>::
call(Ruby_Function_2 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2)
{
  typedef detail::Ruby_Function_2<Fun, T1, T2> RF;
  RF f(fun, t1, t2);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3>
class Ruby_Function_3
{
public:
  Ruby_Function_3(Fun f, T1 const & t1, T2 const & t2, T3 const & t3);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_3 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; 
};

template<typename Fun, typename T1, typename T2, typename T3>
inline Ruby_Function_3<Fun, T1, T2, T3>::
Ruby_Function_3(Fun f, T1 const & t1, T2 const & t2, T3 const & t3)
  : f_(f), t1_(t1), t2_(t2), t3_(t3)
{ }

template<typename Fun, typename T1, typename T2, typename T3>
inline VALUE Ruby_Function_3<Fun, T1, T2, T3>::
operator()()
{
  return f_(t1_, t2_, t3_);
}

template<typename Fun, typename T1, typename T2, typename T3>
inline VALUE Ruby_Function_3<Fun, T1, T2, T3>::
call(Ruby_Function_3 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3)
{
  typedef detail::Ruby_Function_3<Fun, T1, T2, T3> RF;
  RF f(fun, t1, t2, t3);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4>
class Ruby_Function_4
{
public:
  Ruby_Function_4(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_4 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4>
inline Ruby_Function_4<Fun, T1, T2, T3, T4>::
Ruby_Function_4(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4>
inline VALUE Ruby_Function_4<Fun, T1, T2, T3, T4>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4>
inline VALUE Ruby_Function_4<Fun, T1, T2, T3, T4>::
call(Ruby_Function_4 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4)
{
  typedef detail::Ruby_Function_4<Fun, T1, T2, T3, T4> RF;
  RF f(fun, t1, t2, t3, t4);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5>
class Ruby_Function_5
{
public:
  Ruby_Function_5(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_5 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5>
inline Ruby_Function_5<Fun, T1, T2, T3, T4, T5>::
Ruby_Function_5(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5>
inline VALUE Ruby_Function_5<Fun, T1, T2, T3, T4, T5>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5>
inline VALUE Ruby_Function_5<Fun, T1, T2, T3, T4, T5>::
call(Ruby_Function_5 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5)
{
  typedef detail::Ruby_Function_5<Fun, T1, T2, T3, T4, T5> RF;
  RF f(fun, t1, t2, t3, t4, t5);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class Ruby_Function_6
{
public:
  Ruby_Function_6(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_6 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline Ruby_Function_6<Fun, T1, T2, T3, T4, T5, T6>::
Ruby_Function_6(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline VALUE Ruby_Function_6<Fun, T1, T2, T3, T4, T5, T6>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline VALUE Ruby_Function_6<Fun, T1, T2, T3, T4, T5, T6>::
call(Ruby_Function_6 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6)
{
  typedef detail::Ruby_Function_6<Fun, T1, T2, T3, T4, T5, T6> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class Ruby_Function_7
{
public:
  Ruby_Function_7(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_7 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline Ruby_Function_7<Fun, T1, T2, T3, T4, T5, T6, T7>::
Ruby_Function_7(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline VALUE Ruby_Function_7<Fun, T1, T2, T3, T4, T5, T6, T7>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline VALUE Ruby_Function_7<Fun, T1, T2, T3, T4, T5, T6, T7>::
call(Ruby_Function_7 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7)
{
  typedef detail::Ruby_Function_7<Fun, T1, T2, T3, T4, T5, T6, T7> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
class Ruby_Function_8
{
public:
  Ruby_Function_8(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_8 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline Ruby_Function_8<Fun, T1, T2, T3, T4, T5, T6, T7, T8>::
Ruby_Function_8(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline VALUE Ruby_Function_8<Fun, T1, T2, T3, T4, T5, T6, T7, T8>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline VALUE Ruby_Function_8<Fun, T1, T2, T3, T4, T5, T6, T7, T8>::
call(Ruby_Function_8 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8)
{
  typedef detail::Ruby_Function_8<Fun, T1, T2, T3, T4, T5, T6, T7, T8> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class Ruby_Function_9
{
public:
  Ruby_Function_9(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_9 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline Ruby_Function_9<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9>::
Ruby_Function_9(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline VALUE Ruby_Function_9<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline VALUE Ruby_Function_9<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9>::
call(Ruby_Function_9 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9)
{
  typedef detail::Ruby_Function_9<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
class Ruby_Function_10
{
public:
  Ruby_Function_10(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_10 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline Ruby_Function_10<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
Ruby_Function_10(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline VALUE Ruby_Function_10<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline VALUE Ruby_Function_10<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
call(Ruby_Function_10 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10)
{
  typedef detail::Ruby_Function_10<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
class Ruby_Function_11
{
public:
  Ruby_Function_11(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_11 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline Ruby_Function_11<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::
Ruby_Function_11(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline VALUE Ruby_Function_11<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline VALUE Ruby_Function_11<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::
call(Ruby_Function_11 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11)
{
  typedef detail::Ruby_Function_11<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
class Ruby_Function_12
{
public:
  Ruby_Function_12(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_12 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline Ruby_Function_12<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::
Ruby_Function_12(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline VALUE Ruby_Function_12<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline VALUE Ruby_Function_12<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::
call(Ruby_Function_12 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12)
{
  typedef detail::Ruby_Function_12<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
class Ruby_Function_13
{
public:
  Ruby_Function_13(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_13 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_; T13 const & t13_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline Ruby_Function_13<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::
Ruby_Function_13(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12), t13_(t13)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline VALUE Ruby_Function_13<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_, t13_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline VALUE Ruby_Function_13<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::
call(Ruby_Function_13 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13)
{
  typedef detail::Ruby_Function_13<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
class Ruby_Function_14
{
public:
  Ruby_Function_14(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_14 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_; T13 const & t13_; T14 const & t14_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline Ruby_Function_14<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::
Ruby_Function_14(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12), t13_(t13), t14_(t14)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline VALUE Ruby_Function_14<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_, t13_, t14_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline VALUE Ruby_Function_14<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::
call(Ruby_Function_14 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14)
{
  typedef detail::Ruby_Function_14<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------

namespace detail
{

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
class Ruby_Function_15
{
public:
  Ruby_Function_15(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14, T15 const & t15);
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_15 * f);
private:
  Fun f_;
  T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_; T13 const & t13_; T14 const & t14_; T15 const & t15_; 
};

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
inline Ruby_Function_15<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::
Ruby_Function_15(Fun f, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14, T15 const & t15)
  : f_(f), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12), t13_(t13), t14_(t14), t15_(t15)
{ }

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
inline VALUE Ruby_Function_15<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::
operator()()
{
  return f_(t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_, t13_, t14_, t15_);
}

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
inline VALUE Ruby_Function_15<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::
call(Ruby_Function_15 * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
inline VALUE protect(Fun fun, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14, T15 const & t15)
{
  typedef detail::Ruby_Function_15<Fun, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> RF;
  RF f(fun, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15);
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------


} // namespace Rice


#endif // Rice__protect__ipp_

