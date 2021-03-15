#include "rice_traits.hpp"

#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

namespace Rice::detail
{
  template<typename T>
  struct Type<T, std::enable_if_t<is_primitive_v<T>>>
  {
    static constexpr void verify()
    {
      // All good - Rice comes with built-in support for primitive types
    }
  };

  template<typename T>
  constexpr void verifyType()
  {
    Type<T>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  constexpr void verifyTypesImpl()
  {
    (Type<std::tuple_element_t<Is, Tuple_T>>::verify(), ...);
  }

  template<typename Tuple_T>
  constexpr void verifyTypes()
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
}
