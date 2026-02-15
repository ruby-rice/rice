#include <memory>

namespace Rice::detail
{
  template <typename T>
  inline VALUE InstanceRegistry::lookup(T* cppInstance, bool isOwner)
  {
    if (!this->shouldTrack(isOwner))
    {
      return Qnil;
    }

    auto it = this->objectMap_.find((void*)cppInstance);
    return it != this->objectMap_.end() ? it->second : Qnil;
  }

  template <typename T>
  inline void InstanceRegistry::add(T* cppInstance, VALUE rubyInstance, bool isOwner)
  {
    if (!this->shouldTrack(isOwner))
    {
      return;
    }

    this->objectMap_[(void*)cppInstance] = rubyInstance;
  }

  inline void InstanceRegistry::remove(void* cppInstance)
  {
    this->objectMap_.erase(cppInstance);
  }

  inline void InstanceRegistry::clear()
  {
    this->objectMap_.clear();
  }

  inline bool InstanceRegistry::shouldTrack(bool isOwner) const
  {
    switch (this->mode)
    {
      case Mode::Off:
        return false;
      case Mode::Owned:
        return isOwner;
      case Mode::All:
        return true;
      default:
        return false;
    }
  }
}
