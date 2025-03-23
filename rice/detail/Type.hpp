#ifndef Rice__Type__hpp_
#define Rice__Type__hpp_

#include <regex>

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
  std::string cppClassName(const std::string& typeInfoName);
  std::string rubyClassName(const std::string& typeInfoName);
  std::string findGroup(std::string& string, size_t start = 0);
  void replaceGroup(std::string& string, std::regex regex, std::string replacement);
  void replaceAll(std::string& string, std::regex regex, std::string replacement);

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
}

#endif // Rice__Type__hpp_
