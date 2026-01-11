#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

namespace Rice::detail
{
  // ---------- TypeIndexParser ------------
  inline TypeIndexParser::TypeIndexParser(const std::type_index& typeIndex, bool isFundamental) :
    typeIndex_(typeIndex), isFundamental_(isFundamental)
  {
  }

  inline std::string TypeIndexParser::demangle(char const* mangled_name)
  {
#ifdef __GNUC__
    int status = 0;
    char* name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
    if (name)
    {
      return name;
    }
    else
    {
      return mangled_name;
    }
#else
    return mangled_name;
#endif
  }

  inline std::string TypeIndexParser::name()
  {
    return this->demangle(this->typeIndex_.name());
  }

  // Find text inside of < > taking into account nested groups.
  //
  // Example:
  //
  //   std::vector<std::vector<int>, std::allocator<std::vector, std::allocator<int>>>
  inline std::string TypeIndexParser::findGroup(std::string& string, size_t offset)
  {
    int depth = 0;

    auto begin = string.begin() + offset;
    auto start = begin;
    for (auto iter = begin; iter != string.end(); iter++)
    {
      if (*iter == '<')
      {
        if (depth == 0)
        {
          start = iter;
        }
        depth++;
      }
      else if (*iter == '>')
      {
        depth--;
        if (depth == 0)
        {
          // Add + 1 to include current ">" character
          return string.substr(offset + (start - begin), 1 + (iter - start));
        }
        else if (depth < 0)
        {
          throw std::runtime_error("Unbalanced Group");
        }
      }
    }
    throw std::runtime_error("Unbalanced Group");
  }

  inline void TypeIndexParser::replaceAll(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      string = std::regex_replace(string, regex, replacement);
    }
  }

  inline void TypeIndexParser::removeGroup(std::string& string, std::regex regex)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.erase(match.position(), group.length());
    }
  }

  inline void TypeIndexParser::replaceGroup(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.replace(match.position(), group.length(), replacement);
    }
  }

  inline std::string TypeIndexParser::simplifiedName()
  {
    std::string base = this->name();

    // Remove void from Buffer<T, void> - the void comes from SFINAE
    std::regex fixBuffer = std::regex("(Buffer<.*),\\s?void>");
    base = std::regex_replace(base, fixBuffer, "$1>");

    // Remove class keyword
    std::regex classRegex = std::regex("class +");
    base = std::regex_replace(base, classRegex, "");

    // Remove struct keyword
    std::regex structRegex = std::regex("struct +");
    base = std::regex_replace(base, structRegex, "");

    // Remove std::__[^:]*::
    std::regex stdClangRegex = std::regex("std::__[^:]+::");
    base = std::regex_replace(base, stdClangRegex, "std::");

    // Remove allocators
    std::regex allocatorRegex(R"(,\s*std::allocator)");
    removeGroup(base, allocatorRegex);

    // Remove char_traits
    std::regex charTraitsRegex(R"(,\s*std::char_traits)");
    removeGroup(base, charTraitsRegex);

    // Remove less (std::map)
    std::regex lessRegex(R"(,\s*std::less)");
    removeGroup(base, lessRegex);

    // Remove hash (std::unordered_map)
    std::regex hashRegex(R"(,\s*std::hash)");
    removeGroup(base, hashRegex);

    // Remove equal_to (std::unordered_map)
    std::regex equalRegex(R"(,\s*std::equal_to)");
    removeGroup(base, equalRegex);

    // Remove default_delete (std::unique_ptr)
    std::regex defaultDeleteRegex(R"(,\s*std::default_delete)");
    removeGroup(base, defaultDeleteRegex);

    // Remove spaces before pointers
    std::regex ptrRegex = std::regex(R"(\s+\*)");
    base = std::regex_replace(base, ptrRegex, "*");

    // Remove __ptr64
    std::regex ptr64Regex(R"(\s*__ptr64\s*)");
    base = std::regex_replace(base, ptr64Regex, "");

    // Replace " >" with ">"
    std::regex trailingAngleBracketSpaceRegex = std::regex(R"(\s+>)");
    replaceAll(base, trailingAngleBracketSpaceRegex, ">");

    // One space after a comma (MSVC has no spaces, GCC one space)
    std::regex commaSpaceRegex = std::regex(R"(,(\S))");
    replaceAll(base, commaSpaceRegex, ", $1");

    // Fix strings
    std::regex stringRegex = std::regex(R"(basic_string<char>)");
    replaceAll(base, stringRegex, "string");

    std::regex wstringRegex = std::regex(R"(basic_string<wchar_t>)");
    replaceAll(base, wstringRegex, "wstring");

    // Normalize Anonymous namespace
    std::regex anonymousNamespaceGcc = std::regex(R"(\(anonymous namespace\))");
    replaceAll(base, anonymousNamespaceGcc, "AnonymousNamespace");
    std::regex anonymousNamespaceMsvc = std::regex(R"(`anonymous namespace')");
    replaceAll(base, anonymousNamespaceMsvc, "AnonymousNamespace");

    return base;
  }

  inline std::string TypeIndexParser::rubyName(std::string rubyTypeName)
  {
    std::string base = rubyTypeName;

    // Remove std:: these could be embedded in template types
    auto stdRegex = std::regex("std::");
    base = std::regex_replace(base, stdRegex, "");

    // Remove leading namespaces. This will not remove namespaces
    // embedded in template types like std::vector<mynamespace::foo>
    auto leadingNamespacesRegex = std::regex("^[^<]*::");
    base = std::regex_replace(base, leadingNamespacesRegex, "");

    // Capitalize first letter
    base[0] = (char)std::toupper(base[0]);

    // Replace :: with unicode U+u02F8 (Modified Letter raised colon)
    auto colonRegex = std::regex(R"(:)");
    this->replaceAll(base, colonRegex, "\uA789");

    // Replace _ and capitalize the next letter
    std::regex underscoreRegex(R"(_(\w))");
    this->capitalizeHelper(base, underscoreRegex);

    if (this->isFundamental_)
    {
      // Replace space and capitalize the next letter
      std::regex spaceRegex(R"(\s+(\w))");
      this->capitalizeHelper(base, spaceRegex);
    }
    else
    {
      // Replace spaces with unicode U+u00A0 (Non breaking Space)
      std::regex spaceRegex = std::regex(R"(\s+)");
      this->replaceAll(base, spaceRegex, "\u00A0");
    }

    // Replace < with unicode U+227A (Precedes)
    auto lessThanRegex = std::regex("<");
    //replaceAll(base, lessThanRegex, "≺");
    this->replaceAll(base, lessThanRegex, "\u227A");

    // Replace > with unicode U+227B (Succeeds)
    auto greaterThanRegex = std::regex(">");
    //replaceAll(base, greaterThanRegex, "≻");
    this->replaceAll(base, greaterThanRegex, "\u227B");

    // Replace , with Unicode Character (U+066C) - Arabic Thousands Separator
    auto commaRegex = std::regex(R"(,\s*)");
    this->replaceAll(base, commaRegex, "\u201A");

    // Replace * with Unicode Character (U+2217) -	Asterisk Operator
    auto asteriskRegex = std::regex(R"(\*)");
    this->replaceAll(base, asteriskRegex, "\u2217");

    return base;
  }

  inline void TypeIndexParser::capitalizeHelper(std::string& content, std::regex& regex)
  {
    std::smatch match;
    while (std::regex_search(content, match, regex))
    {
      std::string replacement = match[1];
      std::transform(replacement.begin(), replacement.end(), replacement.begin(),
        [](unsigned char c) -> char
        {
          return static_cast<char>(std::toupper(c));
        });
      content.replace(match.position(), match.length(), replacement);
    }
  }

  // ---------- TypeDetail<T> ------------
  template<typename T>
  inline std::type_index TypeDetail<T>::typeIndex()
  {
    if constexpr (is_complete_v<T>)
    {
      return typeid(T);
    }
    else if constexpr (std::is_reference_v<T>)
    {
      // For incomplete reference types, strip the reference and use pointer.
      // Can't use typeid(T&) because it still requires complete type on MSVC.
      return typeid(std::remove_reference_t<T>*);
    }
    else
    {
      return typeid(T*);
    }
  }

  template<typename T>
  inline bool TypeDetail<T>::isFundamental()
  {
    if constexpr (is_complete_v<T>)
    {
      return std::is_fundamental_v<intrinsic_type<T>>;
    }
    else
    {
      return false;
    }
  }

  template<typename T>
  inline std::string TypeDetail<T>::name()
  {
    return this->typeIndexParser_.name();
  }

  template<typename T>
  inline std::string TypeDetail<T>::simplifiedName()
  {
    return this->typeIndexParser_.simplifiedName();
  }

  template<typename T>
  inline std::string TypeDetail<T>::rubyTypeName()
  {
    using Intrinsic_T = detail::intrinsic_type<T>;

    if constexpr (std::is_fundamental_v<T>)
    {
      return RubyType<Intrinsic_T>::name;
    }
    else if constexpr (std::is_same_v<std::remove_cv_t<T>, char*>)
    {
      return "String";
    }
    else
    {
      TypeDetail<Intrinsic_T> typeDetail;
      return typeDetail.simplifiedName();
    }
  }

  template<typename T>
  inline std::string TypeDetail<T>::rubyName()
  {
    std::string base = this->rubyTypeName();
    return this->typeIndexParser_.rubyName(base);
  }

  template<typename T>
  inline VALUE TypeDetail<T>::rubyKlass()
  {
    using Type_T = Type<std::remove_reference_t<detail::remove_cv_recursive_t<T>>>;
    using Intrinsic_T = detail::intrinsic_type<T>;

    if constexpr (has_ruby_klass<Type_T>::value)
    {
      return Type_T::rubyKlass();
    }
    else if constexpr (std::is_fundamental_v<Intrinsic_T> && std::is_pointer_v<T>)
    {
      using Pointer_T = Pointer<std::remove_pointer_t<remove_cv_recursive_t<T>>>;
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Pointer_T>();
      return pair.first;
    }
    else
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Intrinsic_T>();
      return pair.first;
    }
  }
}
