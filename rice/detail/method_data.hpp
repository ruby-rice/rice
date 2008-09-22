#ifndef Rice__method_data__hpp_
#define Rice__method_data__hpp_

#include "ruby.hpp"

namespace Rice
{

namespace detail
{

// Retrieve the data associated with the last called method.
void * method_data();

// Define a method and attach data to it.
void define_method_with_data(
    VALUE klass,
    char const * name,
    RUBY_METHOD_FUNC func,
    int num_args,
    void * data);

// Set the data associated with a method.
void set_method_data(
    VALUE klass,
    char const * name,
    void * data);

} // namespace detail

} // namespace Rice

#endif // Rice__method_data__hpp_
