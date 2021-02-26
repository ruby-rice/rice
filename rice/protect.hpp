#ifndef Rice__protect__hpp_
#define Rice__protect__hpp_

// This causes problems with certain C++ libraries
#undef TYPE

#include "detail/Ruby_Function.hpp"
#include "detail/protect.hpp"
#include "detail/protect.ipp"

namespace Rice
{

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
template<typename Func_T, typename ...Arg_Ts>
VALUE protect(Func_T&& func, const Arg_Ts&... args)
{
  // Create a functor for calling a Ruby fucntion and define some aliases for readability.
  auto rubyFunction = detail::ruby_function(std::forward<Func_T>(func), args...);
  using RubyFunctionType = decltype(rubyFunction);
  VALUE rubyFunctionAsFakeValue = reinterpret_cast<VALUE>(&rubyFunction);

  // Now call rb_protect sending it the functor we created above - it's call method
  // will invoke the underyling Ruby fuction.
  return detail::protect((RUBY_VALUE_FUNC)&RubyFunctionType::call, rubyFunctionAsFakeValue);
}

} // namespace Rice

#endif // Rice__protect__hpp_