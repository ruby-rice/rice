#ifndef Rice__detail__Caster__hpp_
#define Rice__detail__Caster__hpp_

#include "ruby.hpp"
#include <map>

namespace Rice
{

namespace detail
{

class AbstractCaster
{
public:
  static void registerCaster(VALUE from_klass, VALUE to_klass, AbstractCaster* caster);
  static AbstractCaster* find(VALUE from_klass, VALUE to_klass);

public:
  virtual void* cast(void* from) = 0;

private:
  static inline std::map<std::pair<VALUE, VALUE>, AbstractCaster*> registry_;
};

template<typename From_T, typename To_T>
class Caster : public AbstractCaster
{
public:
  void* cast(void* from_void) override;
};

} // detail

} // Rice

#include "Caster.ipp"

#endif //Rice__detail__Caster__hpp_
