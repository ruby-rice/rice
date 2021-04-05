#include "ruby.hpp"
#include "rice_traits.hpp"
#include "from_ruby_defn.hpp"

namespace Rice::detail
{
  template<typename T>
  inline NativeArg<T, typename std::enable_if_t<is_builtin_v<intrinsic_type<T>>>>::NativeArg(Arg& arg) :
    arg_(arg)
  {
  }

  template<typename T>
  inline T NativeArg<T, typename std::enable_if_t<is_builtin_v<intrinsic_type<T>>>>::nativeValue(VALUE value)
  {
    if (value == Qnil && arg_.hasDefaultValue())
    {
      this->native_ = arg_.defaultValue<Intrinsic_T>();
    }
    else
    {
      this->native_ = From_Ruby<Intrinsic_T>::convert(value);
    }

    if constexpr (std::is_pointer_v<T>)
    {
      return &this->native_;
    }
    else
    {
      return this->native_;
    }
  }

  inline NativeArg<char*>::NativeArg(Arg& arg) : arg_(arg)
  {
  }

  inline char* NativeArg<char*>::nativeValue(VALUE value)
  {
    if (value == Qnil && arg_.hasDefaultValue())
    {
      return arg_.defaultValue<char*>();
    }
    else
    {
      return From_Ruby<char*>::convert(value);
    }
  }

  inline NativeArg<VALUE>::NativeArg(Arg& arg) : arg_(arg)
  {
  }

  inline VALUE NativeArg<VALUE>::nativeValue(VALUE value)
  {
    if (arg_.getIsValue())
    {
      return value;
    }
    else
    {
      return From_Ruby<VALUE>::convert(value);
    }
  }

  template<typename T>
  inline NativeArg<T, typename std::enable_if_t<!is_builtin_v<intrinsic_type<T>>>>::NativeArg(Arg& arg) :
    arg_(arg)
  {
  }

  template<typename T>
  inline T NativeArg<T, typename std::enable_if_t<!is_builtin_v<intrinsic_type<T>>>>::nativeValue(VALUE value)
  {
    if (value == Qnil && arg_.hasDefaultValue())
    {
      return arg_.defaultValue<T>();
    }
    else
    {
      return From_Ruby<T>::convert(value);
    }
  }
}