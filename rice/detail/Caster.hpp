#ifndef Rice__detail__Caster__hpp_
#define Rice__detail__Caster__hpp_

#include "ruby.hpp"
#include <any>
#include <map>

namespace Rice
{

namespace detail
{

template<typename To_T>
class CasterAbstract
{
public:
  virtual ~CasterAbstract() = default;
  virtual To_T cast(void* from) = 0;
};

template<typename From_T, typename To_T>
class Caster: public CasterAbstract<To_T>
{
public:
  To_T cast(void* from) override;
};

class CasterRegistry
{
public:
  template <typename To_T>
  static void add(VALUE from_klass, VALUE to_klass, CasterAbstract<To_T>* caster);

  template <typename To_T>
  static CasterAbstract<To_T>* find(VALUE from_klass, VALUE to_klass);

private:
  static inline std::map<std::pair<VALUE, VALUE>, std::any> registry_;
};

} // detail

//! Define an implicit conversion rule between two types.
/*! Given two types, which can be custom types already
 *  wrapped into Rice or fundamental C++ types, this
 *  tells Rice that the two types can be used interchangably.
 *  \param From_T The type to convert from
 *  \param To_T The type to convert to
 */
template<typename From_T, typename To_T>
void define_implicit_cast();

} // Rice

#include "Caster.ipp"

#endif //Rice__detail__Caster__hpp_
