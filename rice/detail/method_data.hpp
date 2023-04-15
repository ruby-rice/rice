#ifndef Rice__detail__method_data__hpp
#define Rice__detail__method_data__hpp

#include <unordered_map>
#include <any>

#include "ruby.hpp"

namespace Rice::detail
{
  class MethodData
  {
  public:
    // Defines a new Ruby method and stores the Rice metadata about it
    template<typename Function_T>
    static void define_method(VALUE klass, ID id, Function_T func, int arity, std::any data);

    // Returns the Rice data for the currently active Ruby method
    template <typename Return_T>
    static Return_T data();

    template <typename Return_T>
    static Return_T data(VALUE klass, ID id);

  private:
    static size_t key(VALUE klass, ID id);
    inline static std::unordered_map<size_t, std::any> methodWrappers_ = {};
  };
} 
#include "method_data.ipp"

#endif // Rice__detail__method_data__hpp