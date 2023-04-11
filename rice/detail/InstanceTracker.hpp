#ifndef Rice__detail__InstanceTracker__hpp_
#define Rice__detail__InstanceTracker__hpp_

#include <map>
#include "ruby.hpp"

namespace Rice::detail
{
  class InstanceTracker
  {
  public:
    template <typename T>
    VALUE lookup(T& cppInstance);

    template <typename T>
    VALUE lookup(T* cppInstance);

    void add(void* cppInstance, VALUE rubyInstance);
    void remove(void* cppInstance);
    void clear();

  public:
    bool isEnabled = false;

  private:
    VALUE lookup(void* cppInstance);
    std::map<void*, VALUE> objectMap_;
  };
} // namespace Rice::detail

#include "InstanceTracker.ipp"

#endif // Rice__detail__InstanceTracker__hpp_

