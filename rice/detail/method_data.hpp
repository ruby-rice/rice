#ifndef method_data__hpp
#define method_data__hpp

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

VALUE define_method_with_data(
    VALUE klass, ID id, VALUE (*cfunc)(ANYARGS), int arity, VALUE data);

VALUE method_data();

} // detail

} // Rice

#endif // method_data__hpp
