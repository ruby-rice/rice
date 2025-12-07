#ifndef Rice__Type__hpp_
#define Rice__Type__hpp_

#include <regex>

RICE_DETAIL_BEGIN_NAMESPACE
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

  class TypeIndexParser
  {
  public:
    TypeIndexParser(const std::type_index& typeIndex, bool isFundamental = false);
    std::string name();
    std::string simplifiedName();
    std::string rubyName(std::string rubyTypeName);

    // public only for testing
    std::string findGroup(std::string& string, size_t start = 0);

  private:
    std::string demangle(char const* mangled_name);
    void removeGroup(std::string& string, std::regex regex);
    void replaceGroup(std::string& string, std::regex regex, std::string replacement);
    void capitalizeHelper(std::string& content, std::regex& regex);
    void replaceAll(std::string& string, std::regex regex, std::string replacement);

  private:
    const std::type_index typeIndex_;
    bool isFundamental_ = false;
  };

  template<typename T>
  class TypeMapper
  {
  public:
    VALUE rubyKlass();
    std::string rubyName();

  private:
    std::string rubyTypeName();
  
  private:
    TypeIndexParser typeIndexParser_{ typeid(T), std::is_fundamental_v<intrinsic_type<T>> };
  };

  template<typename T>
  void verifyType();

  template<typename Tuple_T>
  void verifyTypes();
RICE_DETAIL_END_NAMESPACE

#endif // Rice__Type__hpp_
