#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

namespace Rice::detail
{
  template<typename T>
  bool Type<T>::verify()
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      return true;
    }
    else if constexpr (std::is_array_v<T> && std::is_fundamental_v<std::remove_extent_t<T>>)
    {
      return true;
    }
    else
    {
      return Registries::instance.types.verify<T>();
    }
  }

  template<typename T>
  void verifyType()
  {
    Type<intrinsic_type<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  void verifyTypesImpl(std::index_sequence<Is...> indexes)
  {
    (verifyType<typename std::tuple_element<Is, Tuple_T>::type>(), ...);
  }

  template<typename Tuple_T>
  void verifyTypes()
  {
    std::make_index_sequence<std::tuple_size_v<Tuple_T>> indexes;
    verifyTypesImpl<Tuple_T>(indexes);
  }

  inline std::string demangle(char const* mangled_name)
  {
#ifdef __GNUC__
    struct Helper
    {
      Helper(
        char const* mangled_name)
        : name_(0)
      {
        int status = 0;
        name_ = abi::__cxa_demangle(mangled_name, 0, 0, &status);
      }

      ~Helper()
      {
        std::free(name_);
      }

      char* name_;

    private:
      Helper(Helper const&);
      void operator=(Helper const&);
    };

    Helper helper(mangled_name);
    if (helper.name_)
    {
      return helper.name_;
    }
    else
    {
      return mangled_name;
    }
#else
    return mangled_name;
#endif
  }

  inline std::string typeName(const std::type_index& typeIndex)
  {
    return demangle(typeIndex.name());
  }

  // Find text inside of < > taking into account nested groups.
  // 
  // Example:
  //  
  //   std::vector<std::vector<int>, std::allocator<std::vector, std::allocator<int>>>
  inline std::string findGroup(std::string& string, size_t offset)
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

  inline void replaceAll(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      string = std::regex_replace(string, regex, replacement);
    }
  }

  inline void removeGroup(std::string& string, std::regex regex)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.erase(match.position(), group.length());
    }
  }

  inline void replaceGroup(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.replace(match.position(), group.length(), replacement);
    }
  }

  template<typename T>
  inline std::string cppClassName()
  {
    const std::type_info& typeInfo = typeid(T);
    std::string base = typeName(typeInfo);

    // Remove void from Buffer<T, void> - the void comes from SFINAE
    auto fixBuffer = std::regex("(Buffer<[^,]*),\\s?void>");
    base = std::regex_replace(base, fixBuffer, "$1>");

    // Remove class keyword
    auto classRegex = std::regex("class +");
    base = std::regex_replace(base, classRegex, "");

    // Remove struct keyword
    auto structRegex = std::regex("struct +");
    base = std::regex_replace(base, structRegex, "");

    // Remove std::__[^:]*::
    auto stdClangRegex = std::regex("std::__[^:]+::");
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

    // Remove spaces before pointers
    auto ptrRegex = std::regex(R"(\s+\*)");
    base = std::regex_replace(base, ptrRegex, "*");

    // Remove __ptr64
    std::regex ptr64Regex(R"(\s*__ptr64\s*)");
    base = std::regex_replace(base, ptr64Regex, "");

    // Replace " >" with ">"
    auto trailingAngleBracketSpaceRegex = std::regex(R"(\s+>)");
    replaceAll(base, trailingAngleBracketSpaceRegex, ">");

    // One space after a comma (MSVC has no spaces, GCC one space)
    auto commaSpaceRegex = std::regex(R"(,(\S))");
    replaceAll(base, commaSpaceRegex, ", $1");

    // Fix strings
    auto stringRegex = std::regex(R"(basic_string<char>)");
    replaceAll(base, stringRegex, "string");

    auto wstringRegex = std::regex(R"(basic_string<wchar_t>)");
    replaceAll(base, wstringRegex, "wstring");

    // Normalize Anonymous namespace
    auto anonymousNamespaceGcc = std::regex(R"(\(anonymous namespace\))");
    replaceAll(base, anonymousNamespaceGcc, "AnonymousNamespace");
    auto anonymousNamespaceMsvc = std::regex(R"(`anonymous namespace')");
    replaceAll(base, anonymousNamespaceMsvc, "AnonymousNamespace");

    return base;
  }

  inline void capitalizeHelper(std::string& content, std::regex& regex)
  {
    std::smatch match;
    while (std::regex_search(content, match, regex))
    {
      std::string replacement = match[1];
      std::transform(replacement.begin(), replacement.end(), replacement.begin(), ::toupper);
      content.replace(match.position(), match.length(), replacement);
    }
  }

  template<typename T>
  inline std::string rubyTypeName()
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      return RubyType<intrinsic_type<T>>::name;
    }
    else if constexpr (std::is_same_v<std::remove_cv_t<T>, char*>)
    {
      return "String";
    }
    else if constexpr (std::is_pointer_v<T> && std::is_fundamental_v<detail::intrinsic_type<T>>)
    {
      const std::type_info& typeInfo = typeid(Buffer<T>);
      return cppClassName<Buffer<T>>();
    }
    else
    {
      return cppClassName<intrinsic_type<T>>();
    }
  }

  template<typename T>
  inline std::string rubyClassName()
  {
    std::string base = rubyTypeName<T>();

    // Remove std:: these could be embedded in template types
    auto stdRegex = std::regex("std::");
    base = std::regex_replace(base, stdRegex, "");

    // Remove leading namespaces. This will not remove namespaces
    // embedded in template types like std::vector<mynamespace::foo>
    auto leadingNamespacesRegex = std::regex("^[^<]*::");
    base = std::regex_replace(base, leadingNamespacesRegex, "");

    // Capitalize first letter
    base[0] = std::toupper(base[0]);

    // Replace :: with unicode U+u02F8 (Modified Letter raised colon)
    auto colonRegex = std::regex(R"(:)");
    replaceAll(base, colonRegex, "\uA789");

    // Replace _ and capitalize the next letter
    std::regex underscoreRegex(R"(_(\w))");
    capitalizeHelper(base, underscoreRegex);

    // Replace spaces with unicode U+u00A0 (Non breaking Space)
    auto spaceRegex = std::regex(R"(\s+)");
    replaceAll(base, spaceRegex, "\u00A0");

    // Replace < with unicode U+227A (Precedes)
    auto lessThanRegex = std::regex("<");
    //replaceAll(base, lessThanRegex, "≺");
    replaceAll(base, lessThanRegex, "\u227A");

    // Replace > with unicode U+227B (Succeeds)
    auto greaterThanRegex = std::regex(">");
    //replaceAll(base, greaterThanRegex, "≻");
    replaceAll(base, greaterThanRegex, "\u227B");

    // Replace , with Unicode Character (U+066C) - Arabic Thousands Separator
    auto commaRegex = std::regex(R"(,\s*)");
    replaceAll(base, commaRegex, "\u201A");

    // Replace * with Unicode Character (U+2217) -	Asterisk Operator
    auto asteriskRegex = std::regex(R"(\*)");
    replaceAll(base, asteriskRegex, "\u2217");

    return base;
  }

  template<typename T>
  inline std::string rubyModuleName()
  {
    // Find leading namespaces
    std::string fullName = rubyTypeName<T>();
    auto leadingNamespacesRegex = std::regex("^([^<]*)::");
    std::smatch match;
    if (std::regex_search(fullName, match, leadingNamespacesRegex))
    {
      std::string namespaces = match[1];
      auto capitalizeRegex = std::regex(R"((?=::)(\w))");
      capitalizeHelper(namespaces, capitalizeRegex);
      namespaces[0] = std::toupper(namespaces[0]);
      return namespaces;
    }
    else
    {
      return "";
    }
  }
}