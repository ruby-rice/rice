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

  template<typename T>
  struct Type<T&>
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T&&>
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T*>
  {
    static bool verify();
  };

  template<typename T>
  struct Type<T**>
  {
    static bool verify();
  };

  template<typename T>
  class TypeMapper
  {
  public:
    std::string name();
    std::string name(const std::type_index& typeIndex);
    std::string simplifiedName();
    std::string rubyName();
    VALUE rubyKlass();
  private:
    std::string demangle(char const* mangled_name);
    std::string rubyTypeName();
  };

  std::string TypeMapper_findGroup(std::string& string, size_t start = 0);
  void TypeMapper_replaceAll(std::string& string, std::regex regex, std::string replacement);
  void TypeMapper_removeGroup(std::string& string, std::regex regex);
  void TypeMapper_replaceGroup(std::string& string, std::regex regex, std::string replacement);
  void TypeMapper_simplifiedName(std::string& base);
  void TypeMapper_capitalizeHelper(std::string& content, std::regex& regex);
  void TypeMapper_rubyName(std::string& base, bool is_fundamental_v_intrinsic_type);

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
}

#endif // Rice__Type__hpp_
