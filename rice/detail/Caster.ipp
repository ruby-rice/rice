#include "../Class_defn.hpp"

namespace Rice
{

namespace detail
{
  template<typename From_T, typename To_T>
  inline To_T Caster<From_T, To_T>::cast(void* from)
  {
    From_T* data = static_cast<From_T*>(from);
    return *data;
  }

  template<typename To_T>
  inline void CasterRegistry::add(VALUE from_klass, VALUE to_klass, CasterAbstract<To_T>* caster)
  {
    registry_[std::pair(from_klass, to_klass)] = caster;
  }

  template<typename To_T>
  inline CasterAbstract<To_T>* CasterRegistry::find(VALUE from_klass, VALUE to_klass)
  {
    auto iter = registry_.find(std::pair(from_klass, to_klass));
    if (iter != registry_.end())
    {
      std::any anyCaster = iter->second;
      return std::any_cast<CasterAbstract<To_T>*>(anyCaster);
    }
    else
    {
      return nullptr;
    }
  }
} // detail


template<typename From_T, typename To_T>
inline void
define_implicit_cast()
{
  static_assert(!std::is_pointer_v<From_T>);
  static_assert(!std::is_pointer_v<To_T>);
  static_assert(!std::is_reference_v<From_T>);
  static_assert(!std::is_reference_v<To_T>);

  static_assert(std::is_convertible_v<From_T, To_T>);
  static_assert(!std::is_fundamental_v<From_T>);
  static_assert(!std::is_fundamental_v<To_T>);

  detail::Caster<From_T, To_T>* caster = new detail::Caster<From_T, To_T>();
  Class from_class = Data_Type<From_T>::klass().value();
  Class to_class = Data_Type<To_T>::klass().value();
  detail::CasterRegistry::add(from_class, to_class, caster);
}

} // Rice
