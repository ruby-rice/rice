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

  template<typename From_T, typename To_T>
  inline void CasterRegistry::add(VALUE from_klass, VALUE to_klass)
  {
    static_assert(!std::is_pointer_v<From_T>);
    static_assert(!std::is_pointer_v<To_T>);
    static_assert(!std::is_reference_v<From_T>);
    static_assert(!std::is_reference_v<To_T>);

    static_assert(std::is_convertible_v<From_T, To_T>);
    static_assert(!std::is_fundamental_v<From_T>);
    static_assert(!std::is_fundamental_v<To_T>);

    Caster<From_T, To_T>* caster = new Caster<From_T, To_T>();
    registry_[std::pair(from_klass, to_klass)] = dynamic_cast<CasterAbstract<To_T>*>(caster);
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


} // Rice
