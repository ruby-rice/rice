#include "ruby.hpp"
#include "rice_traits.hpp"
#include "to_ruby.hpp"

namespace Rice::detail
{
  template<typename T>
  inline NativeReturn<T>::NativeReturn(ReturnInfo returnInfo) :
    returnInfo_(returnInfo)
  {
  }

  template<typename T>
  inline VALUE NativeReturn<T>::getValue(T& native)
  {
    return To_Ruby<T>::convert(native, this->returnInfo_.isOwner());
  }

  //template<>
  inline NativeReturn<VALUE>::NativeReturn(ReturnInfo returnInfo) :
    returnInfo_(returnInfo)
  {
  }

  //template<>
  inline VALUE NativeReturn<VALUE>::getValue(VALUE& native)
  {
    if (this->returnInfo_.getIsValue())
    {
      return native;
    }
    else
    {
      return To_Ruby<VALUE>::convert(native, this->returnInfo_.isOwner());
    }
  }

  //template<>
  inline NativeReturn<void>::NativeReturn(ReturnInfo returnInfo) :
    returnInfo_(returnInfo)
  {
  }

  //template<>
  inline VALUE NativeReturn<void>::getValue(VALUE& native)
  {
    return Qnil;
  }
}