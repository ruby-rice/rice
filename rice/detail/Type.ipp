#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

// Rice saves types either as the intrinsic type (MyObject) or pointer (MyObject*).
// It strips out references, const and volatile to avoid an explosion of template classes.
// Pointers are used for C function pointers used in callbacks and for the Buffer class.
namespace Rice::detail
{
  // ------ Type ----------------
  template<typename T>
  inline bool Type<T>::verify()
  {
    return Registries::instance.types.verify<intrinsic_type<T>>();
  }

  template<typename T>
  inline bool Type<T&>::verify()
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      define_pointer<T>();
      define_buffer<T>();
      return true;
    }
    else
    {
      return Type<T>::verify();
    }
  }

  template<typename T>
  inline bool Type<T&&>::verify()
  {
    return Type<T>::verify();
  }

  template<typename T>
  inline bool Type<T*>::verify()
  {
    if constexpr (std::is_fundamental_v<T>)
    {
      define_pointer<T>();
      define_buffer<T>();
      return true;
    }
    else
    {
      return Type<T>::verify();
    }
  }

  template<typename T>
  inline bool Type<T**>::verify()
  {
    define_pointer<T*>();
    define_buffer<T*>();

    if constexpr (std::is_fundamental_v<T>)
    {
      return true;
    }
    else
    {
      return Type<T>::verify();
    }
  }

  template<typename T>
  void verifyType()
  {
    Type<remove_cv_recursive_t<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  void verifyTypesImpl(std::index_sequence<Is...> indexes)
  {
    (verifyType<std::tuple_element_t<Is, Tuple_T>>(), ...);
  }

  template<typename Tuple_T>
  void verifyTypes()
  {
    std::make_index_sequence<std::tuple_size_v<Tuple_T>> indexes;
    verifyTypesImpl<Tuple_T>(indexes);
  }

  // ---------- RubyKlass ------------
  // Helper template to see if the method rubyKlass is defined on a Type specialization
  template<typename, typename = std::void_t<>>
  struct has_ruby_klass : std::false_type
  {
  };

  template<typename T>
  struct has_ruby_klass<T, std::void_t<decltype(T::rubyKlass())>> : std::true_type
  {
  };

  // ---------- TypeMapper ------------
  template<typename T>
  inline std::string TypeMapper<T>::demangle(char const* mangled_name)
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

  template<typename T>
  inline std::string TypeMapper<T>::name()
  {
    const std::type_index& typeIndex = typeid(T);
    return demangle(typeIndex.name());
  }

  template<typename T>
  inline std::string TypeMapper<T>::name(const std::type_index& typeIndex)
  {
    return demangle(typeIndex.name());
  }

  // Find text inside of < > taking into account nested groups.
  // 
  // Example:
  //  
  //   std::vector<std::vector<int>, std::allocator<std::vector, std::allocator<int>>>
  template<typename T>
  inline std::string TypeMapper<T>::findGroup(std::string& string, size_t offset)
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

  template<typename T>
  inline void TypeMapper<T>::replaceAll(std::string& string, std::regex regex, std::string replacement)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      string = std::regex_replace(string, regex, replacement);
    }
  }

  template<typename T>
  inline void TypeMapper<T>::removeGroup(std::string& string, std::regex regex)
  {
    std::smatch match;
    while (std::regex_search(string, match, regex))
    {
      std::string group = findGroup(string, match.position());
      group = match.str() + group;
      string.erase(match.position(), group.length());
    }
  }

  template<typename T>
  inline void TypeMapper<T>::replaceGroup(std::string& string, std::regex regex, std::string replacement)
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
  inline std::string TypeMapper<T>::simplifiedName()
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

  template<typename T>
  inline void TypeMapper<T>::capitalizeHelper(std::string& content, std::regex& regex)
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
  inline std::string TypeMapper<T>::rubyTypeName()
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
      detail::TypeMapper<Intrinsic_T> typeIntrinsicMapper;
      return typeIntrinsicMapper.simplifiedName();
    }
  }

  template<typename T>
  inline std::string TypeMapper<T>::rubyName()
  {
    std::string base = this->rubyTypeName();

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

    if constexpr (std::is_fundamental_v<intrinsic_type<T>>)
    {
      // Replace space and capitalize the next letter
      std::regex spaceRegex(R"(\s+(\w))");
      capitalizeHelper(base, spaceRegex);
    }
    else
    {
      // Replace spaces with unicode U+u00A0 (Non breaking Space)
      std::regex spaceRegex = std::regex(R"(\s+)");
      replaceAll(base, spaceRegex, "\u00A0");
    }

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
  inline VALUE TypeMapper<T>::rubyKlass()
  {
    using Type_T = Type<std::remove_reference_t<detail::remove_cv_recursive_t<T>>>;
    using Intrinsic_T = detail::intrinsic_type<T>;

    // This checks for pointers like int*
    if constexpr (has_ruby_klass<Type_T>::value)
    {
      return Type_T::rubyKlass();
    }
    else
    {
      std::pair<VALUE, rb_data_type_t*> pair = Registries::instance.types.getType<Intrinsic_T>();
      return pair.first;
    }
  }
}