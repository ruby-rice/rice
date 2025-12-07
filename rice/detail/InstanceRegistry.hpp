#ifndef Rice__detail__InstanceRegistry__hpp_
#define Rice__detail__InstanceRegistry__hpp_

#include <map>

RICE_DETAIL_BEGIN_NAMESPACE
  class InstanceRegistry
  {
  public:
    template <typename T>
    VALUE lookup(T& cppInstance);

    template <typename T>
    VALUE lookup(T* cppInstance);
    VALUE lookup(void* cppInstance);

    void add(void* cppInstance, VALUE rubyInstance);
    void remove(void* cppInstance);
    void clear();

  public:
    bool isEnabled = false;

  private:
    std::map<void*, VALUE> objectMap_;
  };
RICE_DETAIL_END_NAMESPACE

#endif // Rice__detail__InstanceRegistry__hpp_

