#ifndef Rice__detail__check_ruby_type__hpp_
#define Rice__detail__check_ruby_type__hpp_

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

// Throws an exception if the given object is not of type klass.
void check_ruby_type(
    VALUE value,
    VALUE klass,
    bool include_super);

} // namespace detail

} // namespace Rice

#endif // Rice__detail__check_ruby_type__hpp_

