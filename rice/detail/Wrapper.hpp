#ifndef Rice__detail__Wrapper__hpp_
#define Rice__detail__Wrapper__hpp_

#include "ruby.hpp"

namespace Rice
{
namespace detail
{

template <typename T>
VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T&& data, bool takeOwnership = true);

template <typename T>
VALUE wrap(VALUE klass, rb_data_type_t* rb_type, T* data, bool takeOwnership = true);

template <typename T>
T* unwrap(VALUE value, rb_data_type_t* rb_type);

void* unwrap(VALUE value);

template <typename T>
static void update(VALUE value, rb_data_type_t* rb_type, T* data, bool takeOwnership = true);

} // namespace detail
} // namespace Rice

#include "Wrapper.ipp"

#endif // Rice__detail__Wrapper__hpp_

