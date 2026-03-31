#ifndef Rice__detail__InstanceRegistry__hpp_
#define Rice__detail__InstanceRegistry__hpp_

#include <type_traits>

#ifdef RICE_RACTOR_SAFE
#include <mutex>
#endif

namespace Rice::detail
{
  class InstanceRegistry
  {
  public:
    enum class Mode
    {
      Off,
      Owned,
      All
    };

    template <typename T>
    VALUE lookup(T* cppInstance, bool isOwner);

    template <typename T>
    void add(T* cppInstance, VALUE rubyInstance, bool isOwner);

    void remove(void* cppInstance);
    void clear();

  public:
    Mode mode = Mode::Owned;

  private:
    bool shouldTrack(bool isOwner) const;

#ifdef RICE_RACTOR_SAFE
    std::recursive_mutex mutex_;
#endif
    std::map<void*, VALUE> objectMap_;
  };
} // namespace Rice::detail

#endif // Rice__detail__InstanceRegistry__hpp_
