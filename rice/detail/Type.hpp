#ifndef Rice__Type__hpp_
#define Rice__Type__hpp_

#include <string>
#include <typeinfo>

namespace Rice::detail
{
  template<typename T, typename = void>
  struct Type
  {
    static constexpr void verify();
    static std::string name2();
  };

  // Return the name of a type
  std::string typeName(const std::type_info& typeInfo);

  template<typename T>
  constexpr void verifyType();

  template<typename Tuple_T>
  constexpr void verifyTypes();
}

#include "Type.ipp"

#endif // Rice__Type__hpp_
