#include "ruby.hpp"
#include "rice_traits.hpp"
#include "Type.hpp"
#include "../Arg.hpp"
#include "from_ruby_defn.hpp"

namespace Rice::detail
{
  template<typename T>
  inline T NativeArg<T, typename std::enable_if_t<is_builtin_v<intrinsic_type<T>>>>::nativeValue(VALUE value)
  {
    this->native_ = From_Ruby<Intrinsic_T>::convert(value);

    if constexpr (std::is_pointer_v<T>)
    {
      return &this->native_;
    }
    else
    {
      return this->native_;
    }
  }

  inline char* NativeArg<char*>::nativeValue(VALUE value)
  {
    return From_Ruby<char*>::convert(value);
  }

  template<typename T>
  inline T NativeArg<T, typename std::enable_if_t<!is_builtin_v<intrinsic_type<T>>>>::nativeValue(VALUE value)
  {
    return From_Ruby<T>::convert(value);
  }
}