#ifndef Rice__detail__creation_funcs__hpp_
#define Rice__detail__creation_funcs__hpp_

namespace Rice
{

class Class;

namespace detail
{

//! Like define_alloc_func, but allows the user to define an
//! "initialize" method too.
template<typename Initialize_Func_T>
void define_creation_funcs(
    Class const & klass,
    RUBY_VALUE_FUNC allocate_func,
    Initialize_Func_T initialize_func);

//! This is just the opposite of define_alloc_func.  It can be
//! used to create a class that cannot be instantiated by the user.
void undef_alloc_func(
    Class const & klass);

//! This is just the opposite of define_creation_func.  It can be
//! used to create a class that cannot be instantiated by the user.
void undef_creation_funcs(
    Class const & klass);

} // namespace detail

} // namespace Rice

#include "creation_funcs.ipp"

#endif // Rice__detail__creation_funcs__hpp_

