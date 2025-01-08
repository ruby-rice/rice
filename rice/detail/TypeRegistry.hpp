#ifndef Rice__TypeRegistry__hpp_
#define Rice__TypeRegistry__hpp_

#include <optional>
#include <unordered_map>
#include <set>
#include <regex>


/* The type registry keeps track of all C++ types wrapped by Rice. When a native function returns 
   an instance of a class/struct we look up its type to verity that it has been registered. 
   
   We have to do this to support C++ inheritance. If a C++ function returns a pointer/reference
   to an Abstract class, the actual returned object will be a Child class. However, all we know
   from the C++ method signature is that it is an Absract class - thus the need for a registry.*/

namespace Rice::detail
{
  class TypeRegistry
  {
  public:
    template <typename T>
    void add(VALUE klass, rb_data_type_t* rbType);

    template <typename T>
    void remove();

    template <typename T>
    bool isDefined();

    template <typename T>
    std::pair<VALUE, rb_data_type_t*> getType();

    template <typename T>
    bool verify();
      
    template <typename T>
    std::pair<VALUE, rb_data_type_t*> figureType(const T& object);

    // Validate types and throw if any types are unverified
    void validateTypes();

    // Clear unverified types. This is mostly for unit tests
    void clearUnverifiedTypes();

  private:
    std::optional<std::pair<VALUE, rb_data_type_t*>> lookup(const std::type_info& typeInfo);
    void raiseUnverifiedType(const std::string& typeName);

    std::unordered_map<std::type_index, std::pair<VALUE, rb_data_type_t*>> registry_{};
    std::set<std::type_index> unverified_{};
  };
}

#endif // Rice__TypeRegistry__hpp_
