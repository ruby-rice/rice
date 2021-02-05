namespace Rice
{

namespace detail
{

inline void AbstractCaster::registerCaster(VALUE from_klass, VALUE to_klass, AbstractCaster* caster)
{
  registry_[std::pair(from_klass, to_klass)] = caster;
}

inline AbstractCaster* AbstractCaster::find(VALUE from_klass, VALUE to_klass)
{
  auto iter = registry_.find(std::pair(from_klass, to_klass));
  if (iter != registry_.end())
  {
    return iter->second;
  }
  else
  {
    return nullptr;
  }
}

template <typename From_T, typename To_T>
inline void* Caster<From_T, To_T>::cast(void* from_void)
{
  From_T* from = static_cast<From_T*>(from_void);
  // TODO - this is a memory leak!!!! Should we return a unique pointer?
  To_T* to = new To_T(*from);
  return static_cast<void*>(to);
}

} // detail

} // Rice
