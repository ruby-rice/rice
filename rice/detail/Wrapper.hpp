#ifndef Rice__detail__Wrapper__hpp_
#define Rice__detail__Wrapper__hpp_

#include "ruby.hpp"

namespace Rice
{
namespace detail
{

template <typename T>
VALUE wrap(VALUE klass, RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free, T&& data, bool takeOwnership = true);

template <typename T>
VALUE wrap(VALUE klass, RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free, T* data, bool takeOwnership = true);

template <typename T>
T* unwrap(VALUE value);

template <typename T>
static void update(VALUE value, T* data, bool takeOwnership = true);

} // namespace detail
} // namespace Rice

#include "Wrapper.ipp"

#endif // Rice__detail__Wrapper__hpp_

