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

    // public only for testing
    std::string findGroup(std::string& string, size_t start = 0);
  private:
    std::string demangle(char const* mangled_name);
    std::string rubyTypeName();
    void removeGroup(std::string& string, std::regex regex);
    void replaceGroup(std::string& string, std::regex regex, std::string replacement);
    void replaceAll(std::string& string, std::regex regex, std::string replacement);
    void capitalizeHelper(std::string& content, std::regex& regex);
  };

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
}

#endif // Rice__Type__hpp_
