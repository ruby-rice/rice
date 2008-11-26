#ifndef Rice__detail__method_data__hpp
#define Rice__detail__method_data__hpp

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

VALUE define_method_with_data(
    VALUE klass, ID id, VALUE (*cfunc)(ANYARGS), int arity, VALUE data);

VALUE method_data();

} // namespace detail

} // namespace Rice

#endif // Rice__detail__method_data__hpp
