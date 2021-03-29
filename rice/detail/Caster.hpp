#ifndef Rice__detail__Caster__hpp_
#define Rice__detail__Caster__hpp_

#include "ruby.hpp"
#include <any>
#include <map>

namespace Rice::detail
{
  template<typename To_T>
  class CasterAbstract
  {
  public:
    virtual ~CasterAbstract() = default;
    virtual To_T cast(void* from) = 0;
  };

  template<typename From_T, typename To_T>
  class Caster : public CasterAbstract<To_T>
  {
  public:
    To_T cast(void* from) override;
  };

  class CasterRegistry
  {
  public:
    template<typename From_T, typename To_T>
    static void add(VALUE from_klass, VALUE to_klass);

    template <typename To_T>
    static CasterAbstract<To_T>* find(VALUE from_klass, VALUE to_klass);

  private:
    static inline std::map<std::pair<VALUE, VALUE>, std::any> registry_;
  };
}
#include "Caster.ipp"

#endif //Rice__detail__Caster__hpp_