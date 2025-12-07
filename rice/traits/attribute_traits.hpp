#ifndef Rice__detail__attribute_traits__hpp_
#define Rice__detail__attribute_traits__hpp_

#include <tuple>

RICE_DETAIL_BEGIN_NAMESPACE
  // Base class
  template<typename Attribute_T>
  struct attribute_traits;

  template<typename Attribute_T>
  struct attribute_traits<Attribute_T*>
  {
    using attr_type = Attribute_T;
    using class_type = std::nullptr_t;
  };

  template<typename Attribute_T, typename Class_T>
  struct attribute_traits<Attribute_T(Class_T::*)>
  {
    using attr_type = Attribute_T;
    using class_type = Class_T;
  };
RICE_DETAIL_END_NAMESPACE
#endif // Rice__detail__attribute_traits__hpp_
