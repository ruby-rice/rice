#ifndef Rice__detail__attribute_traits__hpp_
#define Rice__detail__attribute_traits__hpp_

#include <tuple>

namespace Rice::detail
{
  // Base class
  template<typename Attribute_T>
  struct attribute_traits;

  template<typename Attr_T>
  struct attribute_traits<Attr_T*>
  {
    using attr_type = Attr_T;
    using class_type = std::nullptr_t;
  };

  template<typename Attr_T, typename Class_T>
  struct attribute_traits<Attr_T Class_T::*> 
  {
    using attr_type = Attr_T;
    using class_type = Class_T;
  };
}
#endif // Rice__detail__attribute_traits__hpp_
