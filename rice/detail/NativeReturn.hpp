#ifndef Rice__detail__native_return__hpp_
#define Rice__detail__native_return__hpp_

#include "ruby.hpp"
#include "rice_traits.hpp"
#include "../ReturnInfo.hpp"

namespace Rice::detail
{
  // Primary Template for NativeReturn
  template <typename T>
  class NativeReturn
  {
  public:
    NativeReturn(ReturnInfo returnInfo);
    VALUE getValue(T& native);

  private:
    ReturnInfo returnInfo_;
  };

  // Special case VALUE which could be a 64 bit integer or Ruby value.
  template <>
  class NativeReturn<VALUE>
  {
  public:
    NativeReturn(ReturnInfo returnInfo);
    VALUE getValue(VALUE& native);

  private:
    ReturnInfo returnInfo_;
  };

  // Special case void which should never happen due to if constexpr check but nevertheless MSVC
  // seems to compile
  template <>
  class NativeReturn<void>
  {
  public:
    NativeReturn(ReturnInfo returnInfo);
    VALUE getValue(VALUE& native);

  private:
    ReturnInfo returnInfo_;
  };
}

#include "NativeReturn.ipp"

#endif // Rice__detail__native_return__hpp_
