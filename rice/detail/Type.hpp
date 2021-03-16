#ifndef Rice__Type__hpp_
#define Rice__Type__hpp_

#include <string>
#include <typeinfo>
#include "rice_traits.hpp"

namespace Rice::detail
{
  template<typename T>
  struct Type
  {
    static constexpr void verify();
  };

  // Return the name of a type
  std::string typeName(const std::type_info& typeInfo);

  template<typename T>
  constexpr void verifyType();

  template<typename Tuple_T>
  constexpr void verifyTypes();
}

#endif // Rice__Type__hpp_
