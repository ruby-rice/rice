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

  template<typename Ret_T, typename Fun>
  class Ruby_Function_0
  {
  public:
    Ruby_Function_0(Fun f);
    inline void operator()();
    static inline VALUE call(Ruby_Function_0 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun>
  inline Ruby_Function_0<Ret_T, Fun>::
  Ruby_Function_0(Fun f)
    : f_(f)
  { }

  template<typename Ret_T, typename Fun>
  inline void Ruby_Function_0<Ret_T, Fun>::
  operator()()
  {
    result_ = f_();
  }

  template<typename Ret_T, typename Fun>
  inline VALUE Ruby_Function_0<Ret_T, Fun>::
  call(Ruby_Function_0 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

template<typename Ret_T, typename Fun>
inline Ret_T protect(Fun fun)
{
  typedef detail::Ruby_Function_0<Ret_T, Fun> RF;
  RF f(fun);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

template<typename Fun>
inline VALUE protect(Fun fun)
{
  typedef detail::Ruby_Function_0<VALUE, Fun> RF;
  RF f(fun);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------

namespace detail
{

  template<typename Ret_T, typename Fun, typename T0>
  class Ruby_Function_1
  {
  public:
    Ruby_Function_1(Fun f, T0 const & t0);
    inline void operator()();
    static inline VALUE call(Ruby_Function_1 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0>
  inline Ruby_Function_1<Ret_T, Fun, T0>::
  Ruby_Function_1(Fun f, T0 const & t0)
    : f_(f), t0_(t0)
  { }

  template<typename Ret_T, typename Fun, typename T0>
  inline void Ruby_Function_1<Ret_T, Fun, T0>::
  operator()()
  {
    result_ = f_(t0_);
  }

  template<typename Ret_T, typename Fun, typename T0>
  inline VALUE Ruby_Function_1<Ret_T, Fun, T0>::
  call(Ruby_Function_1 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0>
inline Ret_T protect(Fun fun, T0 const & t0)
{
  typedef detail::Ruby_Function_1<Ret_T, Fun, T0> RF;
  RF f(fun, t0);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0>
inline VALUE protect(Fun fun, T0 const & t0)
{
  typedef detail::Ruby_Function_1<VALUE, Fun, T0> RF;
  RF f(fun, t0);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1>
  class Ruby_Function_2
  {
  public:
    Ruby_Function_2(Fun f, T0 const & t0, T1 const & t1);
    inline void operator()();
    static inline VALUE call(Ruby_Function_2 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1>
  inline Ruby_Function_2<Ret_T, Fun, T0, T1>::
  Ruby_Function_2(Fun f, T0 const & t0, T1 const & t1)
    : f_(f), t0_(t0), t1_(t1)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1>
  inline void Ruby_Function_2<Ret_T, Fun, T0, T1>::
  operator()()
  {
    result_ = f_(t0_, t1_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1>
  inline VALUE Ruby_Function_2<Ret_T, Fun, T0, T1>::
  call(Ruby_Function_2 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1)
{
  typedef detail::Ruby_Function_2<Ret_T, Fun, T0, T1> RF;
  RF f(fun, t0, t1);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1)
{
  typedef detail::Ruby_Function_2<VALUE, Fun, T0, T1> RF;
  RF f(fun, t0, t1);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2>
  class Ruby_Function_3
  {
  public:
    Ruby_Function_3(Fun f, T0 const & t0, T1 const & t1, T2 const & t2);
    inline void operator()();
    static inline VALUE call(Ruby_Function_3 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2>
  inline Ruby_Function_3<Ret_T, Fun, T0, T1, T2>::
  Ruby_Function_3(Fun f, T0 const & t0, T1 const & t1, T2 const & t2)
    : f_(f), t0_(t0), t1_(t1), t2_(t2)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2>
  inline void Ruby_Function_3<Ret_T, Fun, T0, T1, T2>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2>
  inline VALUE Ruby_Function_3<Ret_T, Fun, T0, T1, T2>::
  call(Ruby_Function_3 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2)
{
  typedef detail::Ruby_Function_3<Ret_T, Fun, T0, T1, T2> RF;
  RF f(fun, t0, t1, t2);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2)
{
  typedef detail::Ruby_Function_3<VALUE, Fun, T0, T1, T2> RF;
  RF f(fun, t0, t1, t2);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3>
  class Ruby_Function_4
  {
  public:
    Ruby_Function_4(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3);
    inline void operator()();
    static inline VALUE call(Ruby_Function_4 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3>
  inline Ruby_Function_4<Ret_T, Fun, T0, T1, T2, T3>::
  Ruby_Function_4(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3>
  inline void Ruby_Function_4<Ret_T, Fun, T0, T1, T2, T3>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3>
  inline VALUE Ruby_Function_4<Ret_T, Fun, T0, T1, T2, T3>::
  call(Ruby_Function_4 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3)
{
  typedef detail::Ruby_Function_4<Ret_T, Fun, T0, T1, T2, T3> RF;
  RF f(fun, t0, t1, t2, t3);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3)
{
  typedef detail::Ruby_Function_4<VALUE, Fun, T0, T1, T2, T3> RF;
  RF f(fun, t0, t1, t2, t3);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
  class Ruby_Function_5
  {
  public:
    Ruby_Function_5(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4);
    inline void operator()();
    static inline VALUE call(Ruby_Function_5 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
  inline Ruby_Function_5<Ret_T, Fun, T0, T1, T2, T3, T4>::
  Ruby_Function_5(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
  inline void Ruby_Function_5<Ret_T, Fun, T0, T1, T2, T3, T4>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
  inline VALUE Ruby_Function_5<Ret_T, Fun, T0, T1, T2, T3, T4>::
  call(Ruby_Function_5 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4)
{
  typedef detail::Ruby_Function_5<Ret_T, Fun, T0, T1, T2, T3, T4> RF;
  RF f(fun, t0, t1, t2, t3, t4);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4)
{
  typedef detail::Ruby_Function_5<VALUE, Fun, T0, T1, T2, T3, T4> RF;
  RF f(fun, t0, t1, t2, t3, t4);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
  class Ruby_Function_6
  {
  public:
    Ruby_Function_6(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5);
    inline void operator()();
    static inline VALUE call(Ruby_Function_6 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline Ruby_Function_6<Ret_T, Fun, T0, T1, T2, T3, T4, T5>::
  Ruby_Function_6(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline void Ruby_Function_6<Ret_T, Fun, T0, T1, T2, T3, T4, T5>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline VALUE Ruby_Function_6<Ret_T, Fun, T0, T1, T2, T3, T4, T5>::
  call(Ruby_Function_6 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5)
{
  typedef detail::Ruby_Function_6<Ret_T, Fun, T0, T1, T2, T3, T4, T5> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5)
{
  typedef detail::Ruby_Function_6<VALUE, Fun, T0, T1, T2, T3, T4, T5> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  class Ruby_Function_7
  {
  public:
    Ruby_Function_7(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6);
    inline void operator()();
    static inline VALUE call(Ruby_Function_7 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  inline Ruby_Function_7<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6>::
  Ruby_Function_7(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  inline void Ruby_Function_7<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  inline VALUE Ruby_Function_7<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6>::
  call(Ruby_Function_7 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6)
{
  typedef detail::Ruby_Function_7<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6)
{
  typedef detail::Ruby_Function_7<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
  class Ruby_Function_8
  {
  public:
    Ruby_Function_8(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7);
    inline void operator()();
    static inline VALUE call(Ruby_Function_8 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
  inline Ruby_Function_8<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7>::
  Ruby_Function_8(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
  inline void Ruby_Function_8<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
  inline VALUE Ruby_Function_8<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7>::
  call(Ruby_Function_8 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7)
{
  typedef detail::Ruby_Function_8<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7)
{
  typedef detail::Ruby_Function_8<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
  class Ruby_Function_9
  {
  public:
    Ruby_Function_9(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8);
    inline void operator()();
    static inline VALUE call(Ruby_Function_9 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
  inline Ruby_Function_9<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8>::
  Ruby_Function_9(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
  inline void Ruby_Function_9<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
  inline VALUE Ruby_Function_9<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8>::
  call(Ruby_Function_9 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8)
{
  typedef detail::Ruby_Function_9<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8)
{
  typedef detail::Ruby_Function_9<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
  class Ruby_Function_10
  {
  public:
    Ruby_Function_10(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9);
    inline void operator()();
    static inline VALUE call(Ruby_Function_10 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
  inline Ruby_Function_10<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::
  Ruby_Function_10(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
  inline void Ruby_Function_10<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
  inline VALUE Ruby_Function_10<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::
  call(Ruby_Function_10 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9)
{
  typedef detail::Ruby_Function_10<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9)
{
  typedef detail::Ruby_Function_10<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
  class Ruby_Function_11
  {
  public:
    Ruby_Function_11(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10);
    inline void operator()();
    static inline VALUE call(Ruby_Function_11 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
  inline Ruby_Function_11<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
  Ruby_Function_11(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
  inline void Ruby_Function_11<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
  inline VALUE Ruby_Function_11<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
  call(Ruby_Function_11 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10)
{
  typedef detail::Ruby_Function_11<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10)
{
  typedef detail::Ruby_Function_11<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
  class Ruby_Function_12
  {
  public:
    Ruby_Function_12(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11);
    inline void operator()();
    static inline VALUE call(Ruby_Function_12 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
  inline Ruby_Function_12<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::
  Ruby_Function_12(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
  inline void Ruby_Function_12<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
  inline VALUE Ruby_Function_12<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::
  call(Ruby_Function_12 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11)
{
  typedef detail::Ruby_Function_12<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11)
{
  typedef detail::Ruby_Function_12<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
  class Ruby_Function_13
  {
  public:
    Ruby_Function_13(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12);
    inline void operator()();
    static inline VALUE call(Ruby_Function_13 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
  inline Ruby_Function_13<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::
  Ruby_Function_13(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
  inline void Ruby_Function_13<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
  inline VALUE Ruby_Function_13<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::
  call(Ruby_Function_13 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12)
{
  typedef detail::Ruby_Function_13<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12)
{
  typedef detail::Ruby_Function_13<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
  class Ruby_Function_14
  {
  public:
    Ruby_Function_14(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13);
    inline void operator()();
    static inline VALUE call(Ruby_Function_14 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_; T13 const & t13_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
  inline Ruby_Function_14<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::
  Ruby_Function_14(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12), t13_(t13)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
  inline void Ruby_Function_14<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_, t13_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
  inline VALUE Ruby_Function_14<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::
  call(Ruby_Function_14 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13)
{
  typedef detail::Ruby_Function_14<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13)
{
  typedef detail::Ruby_Function_14<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
namespace detail
{

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
  class Ruby_Function_15
  {
  public:
    Ruby_Function_15(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14);
    inline void operator()();
    static inline VALUE call(Ruby_Function_15 * f);
    Ret_T const & result() const { return result_; }

  private:
    Fun f_;
    T0 const & t0_; T1 const & t1_; T2 const & t2_; T3 const & t3_; T4 const & t4_; T5 const & t5_; T6 const & t6_; T7 const & t7_; T8 const & t8_; T9 const & t9_; T10 const & t10_; T11 const & t11_; T12 const & t12_; T13 const & t13_; T14 const & t14_;
    Ret_T result_; // TODO: use placement new
  };

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
  inline Ruby_Function_15<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::
  Ruby_Function_15(Fun f, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14)
    : f_(f), t0_(t0), t1_(t1), t2_(t2), t3_(t3), t4_(t4), t5_(t5), t6_(t6), t7_(t7), t8_(t8), t9_(t9), t10_(t10), t11_(t11), t12_(t12), t13_(t13), t14_(t14)
  { }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
  inline void Ruby_Function_15<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::
  operator()()
  {
    result_ = f_(t0_, t1_, t2_, t3_, t4_, t5_, t6_, t7_, t8_, t9_, t10_, t11_, t12_, t13_, t14_);
  }

  template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
  inline VALUE Ruby_Function_15<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::
  call(Ruby_Function_15 * f)
  {
    (*f)();
    return Qnil;
  }

} // namespace detail

/**
 * protect on a function with a return type
 */
template<typename Ret_T, typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline Ret_T protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14)
{
  typedef detail::Ruby_Function_15<Ret_T, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

/**
 * protect on a function with VALUE return type
 */
template<typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline VALUE protect(Fun fun, T0 const & t0, T1 const & t1, T2 const & t2, T3 const & t3, T4 const & t4, T5 const & t5, T6 const & t6, T7 const & t7, T8 const & t8, T9 const & t9, T10 const & t10, T11 const & t11, T12 const & t12, T13 const & t13, T14 const & t14)
{
  typedef detail::Ruby_Function_15<VALUE, Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14> RF;
  RF f(fun, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14);
  detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
  return f.result();
}

// ---------------------------------------------------------------------
} // namespace Rice


#endif // Rice__protect__ipp_

