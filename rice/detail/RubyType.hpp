#ifndef Rice__detail__ruby__type__hpp_
#define Rice__detail__ruby__type__hpp_

#include <set>

namespace Rice::detail
{
  template <typename T>
  class RubyType
  {
  public:
    static std::set<ruby_value_type> types();
    static std::set<ruby_value_type> convertibleFrom();
    static T(*converter)(VALUE);
    static std::string packTemplate;
  };
}

#endif // Rice__detail__ruby__type__hpp_
