#include <memory>

namespace Rice::detail
{
  template <typename T>
  inline VALUE InstanceTracker::lookup(T& cppInstance)
  {
    return this->lookup((void*)&cppInstance);
  }

  template <typename T>
  inline VALUE InstanceTracker::lookup(T* cppInstance)
  {
    return this->lookup((void*)cppInstance);
  }

  inline VALUE InstanceTracker::lookup(void* cppInstance)
  {
    if (!this->isEnabled)
      return Qnil;

    auto it = this->objectMap_.find(cppInstance);
    if (it != this->objectMap_.end())
    {
      return it->second;
    }
    else
    {
      return Qnil;
    }
  }

  inline void InstanceTracker::add(void* cppInstance, VALUE rubyInstance)
  {
    if (this->isEnabled)
    {
      this->objectMap_[cppInstance] = rubyInstance;
    }
  }

  inline void InstanceTracker::remove(void* cppInstance)
  {
    this->objectMap_.erase(cppInstance);
  }

  inline void InstanceTracker::clear()
  {
    this->objectMap_.clear();
  }

  inline InstanceTracker INSTANCE_TRACKER;

} // namespace
