#ifndef Rice__Type__hpp_
#define Rice__Type__hpp_

#include <string>
#include <typeinfo>
#include <typeindex>

namespace Rice::detail
{
  template<typename T>
  struct Type
  {
    static bool verify();
  };

  // Return the name of a type
  std::string typeName(const std::type_info& typeInfo);
  std::string typeName(const std::type_index& typeIndex);
  std::string makeClassName(const std::type_info& typeInfo);

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
}

#endif // Rice__Type__hpp_
