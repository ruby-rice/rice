#ifndef Rice__Type__ipp_
#define Rice__Type__ipp_

#include "rice_traits.hpp"

#include <tuple>

#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <cstring>
#endif

namespace Rice::detail
{
  // In theory this could be done a separate template specializations using SFINAE. However
  // when I tried that, and then added specialations for std::unique_ptr<T>, MSVC and GCC
  // both complained that two specializations matched. Not sure why...sigh.
  // 
  // Note T *must* be an intrinsic_type so that we don't have to define specializations
  // for pointers, references, const, etc.
  template<typename T>
  constexpr void Type<T>::verify()
  {
    if constexpr (is_primitive_v<T>)
    {
      // Do nothing
    }
    else if constexpr (is_kind_of_object<T>)
    {
      // Do nothing
    }
    else if constexpr (std::is_enum_v<T>)
    {
      Enum<T>::verify();
    }
    else
    {
      Data_Type<T>::verify();
    }
  }

  template<>
  struct Type<void>
  {
    static constexpr void verify()
    {
      // Nothing to verify
    }
  };

  template<typename T>
  constexpr void verifyType()
  {
    Type<intrinsic_type<T>>::verify();
  }

  template<typename Tuple_T, size_t...Is>
  constexpr void verifyTypesImpl()
  {
    (Type<intrinsic_type<std::tuple_element_t<Is, Tuple_T>>>::verify(), ...);
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

#endif //Rice__Type__ipp_