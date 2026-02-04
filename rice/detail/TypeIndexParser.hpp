#ifndef Rice__detail__TypeIndexParser__hpp_
#define Rice__detail__TypeIndexParser__hpp_

#include <regex>
#include <typeindex>

namespace Rice::detail
{
  class TypeIndexParser
  {
  public:
    TypeIndexParser(const std::type_index& typeIndex, bool isFundamental = false);
    std::string name();
    std::string simplifiedName();
    std::string rubyName(std::string rubyTypeName);

    // public only for testing
    std::string findGroup(std::string& string, size_t start = 0);

  protected:
    std::string demangle(char const* mangled_name);
    void removeGroup(std::string& string, std::regex regex);
    void replaceGroup(std::string& string, std::regex regex, std::string replacement);
    void capitalizeHelper(std::string& content, std::regex& regex);
    void replaceAll(std::string& string, std::regex regex, std::string replacement);

  protected:
    const std::type_index typeIndex_;
    bool isFundamental_ = false;
  };

  template<typename T>
  class TypeDetail
  {
  public:
    std::string name();
    std::string simplifiedName();

    VALUE rubyKlass();
    std::string rubyName();

  private:
    static std::type_index typeIndex();
    static bool isFundamental();
    std::string rubyTypeName();

  private:
    TypeIndexParser typeIndexParser_{ typeIndex(), isFundamental() };
  };
}

#endif // Rice__detail__TypeIndexParser__hpp_
