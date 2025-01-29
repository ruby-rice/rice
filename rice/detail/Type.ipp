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
    else
    {
      return Registries::instance.types.verify<T>();
    }
  }

  template<>
  struct Type<void>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<typename T>
  void verifyType()
  {
    Type<intrinsic_type<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  void verifyTypesImpl()
  {
    (verifyType<typename std::tuple_element<Is, Tuple_T>::type>(), ...);
  }

  template<typename Tuple_T>
  void verifyTypes()
  {
    if constexpr (std::tuple_size<Tuple_T>::value > 0)
    {
      verifyTypesImpl<Tuple_T, std::tuple_size<Tuple_T>::value - 1>();
    }
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

  inline std::string typeName(const std::type_info& typeInfo)
  {
    return demangle(typeInfo.name());
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

  inline std::string makeClassName(const std::string& typeInfoName)
  {
    std::string base = typeInfoName;

    // Remove class keyword
    auto classRegex = std::regex("class +");
    base = std::regex_replace(typeInfoName, classRegex, "");

    // Remove struct keyword
    auto structRegex = std::regex("struct +");
    base = std::regex_replace(base, structRegex, "");

    // Remove std::__[^:]*::
    auto stdClangRegex = std::regex("std::__[^:]+::");
    base = std::regex_replace(base, stdClangRegex, "");
      
    // Remove std::
    auto stdRegex = std::regex("std::");
    base = std::regex_replace(base, stdRegex, "");

    // Replace basic_string with string
    auto basicStringRegex = std::regex(R"(basic_string)");
    replaceAll(base, basicStringRegex, "string");
      
    // Remove allocators
    std::regex allocatorRegex(R"(,\s*allocator)");
    removeGroup(base, allocatorRegex);

    // Remove char_traits
    std::regex charTraitsRegex(R"(,\s*char_traits)");
    removeGroup(base, charTraitsRegex);

    // Remove less (std::map)
    std::regex lessRegex(R"(,\s*less)");
    removeGroup(base, lessRegex);

    // Remove hash (std::unordered_map)
    std::regex hashRegex(R"(,\s*hash)");
    removeGroup(base, hashRegex);

    // Remove equal_to (std::unordered_map)
    std::regex equalRegex(R"(,\s*equal_to)");
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

    // Capitalize first letter
    base[0] = std::toupper(base[0]);

    // Replace :: with unicode U+u02F8 (Modified Letter raised colon)
    auto colonRegex = std::regex(R"(:)");
    replaceAll(base, colonRegex, "\uA789");

    // Replace _ and capitalize the next letter
    std::regex namespaceRegex(R"(_(\w))");
    std::smatch namespaceMatch;
    while (std::regex_search(base, namespaceMatch, namespaceRegex))
    {
      std::string replacement = namespaceMatch[1];
      std::transform(replacement.begin(), replacement.end(), replacement.begin(), ::toupper);
      base.replace(namespaceMatch.position(), namespaceMatch.length(), replacement);
    }

    // Replace spaces with unicode U+u00A0 (Non breaking Space)
    auto spaceRegex = std::regex(R"(\s+)");
    replaceAll(base, spaceRegex, "\u00A0");

    // Replace < with unicode U+227A (Precedes)
    auto lessThanRegex = std::regex("<");
    //replaceAll(base, lessThanRegex, u8"≺");
    replaceAll(base, lessThanRegex, "\u227A");

    // Replace > with unicode U+227B (Succeeds)
    auto greaterThanRegex = std::regex(">");
    //replaceAll(base, greaterThanRegex, u8"≻");
    replaceAll(base, greaterThanRegex, "\u227B");

    // Replace , with Unicode Character (U+066C) - Arabic Thousands Separator
    auto commaRegex = std::regex(R"(,\s*)");
    replaceAll(base, commaRegex, "\u201A");

    return base;
  }
}
