#ifndef Rice__detail__NativeRegistry__hpp
#define Rice__detail__NativeRegistry__hpp

#include <memory>
#include <utility>

/* The Native Registry tracks C++ instance that are used to invoke C++ methods for Ruby. 
   These objects include instances of the NativeFunction, NativeIterator, NativeAttributeGet
   and NativeAttributeSet Each instance is specialized to call a specific C++ function, method
   or attribute that is exposed to Ruby.
   
   The registry stores these C++ instances using a map of vectors. The map is keyed on the 
   the Ruby class (VALUE) and method id (ID). The value is a vector of Native pointers stored
   in a std::unique_ptr. Thus the registry takes ownership of the pointers when calling
   code adds them to the registry. The value is a vector to support C++ method overloading.
   
   Note - when an existing Ruby class is redefined using rb_define_class, its VALUE stays the same
   but all its methods and fields are reset. Thus any call to rb_define_class must be followed
   by calling the reset method on the registry. Although redefinition shouldn't happen in 
   production code it happens in many places in the unit tests. */
   
namespace Rice::detail
{
  class NativeRegistry
  {
  public:
    // std::is_copy_constructible returns true for std::vector<std::unique_ptr> - so we need
    // to force the issue
    NativeRegistry() = default;
    NativeRegistry(const NativeRegistry& other) = delete;
    NativeRegistry& operator=(const NativeRegistry& other) = delete;

    void add(VALUE klass, ID methodId, std::unique_ptr<Native>& native);
    void replace(VALUE klass, ID methodId, std::unique_ptr<Native>& native);
    void reset(VALUE klass);

    std::vector<Native*> lookup(VALUE klass);
    std::vector<std::unique_ptr<Native>>& lookup(VALUE klass, ID methodId);
    std::vector<Native*> lookup(VALUE klass, NativeKind kind);

  private:
    // Key - Ruby klass/method
    // Value - Vector of Native pointers owned by the registry (thus wrapped in std::unique_ptr)
    std::map<std::pair<VALUE, ID>, std::vector<std::unique_ptr<Native>>> natives_ = {};
  };
}

#endif // Rice__detail__NativeRegistry__hpp
