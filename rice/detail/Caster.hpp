#ifndef Rice__detail__Caster__hpp_
#define Rice__detail__Caster__hpp_

#include "../Module.hpp"
#include <stdexcept>

namespace Rice
{

namespace detail
{

class Abstract_Caster
{
public:
  virtual void * cast_to_base(void * derived, Module type) const = 0;
  virtual ~Abstract_Caster() { }
};

template<typename Derived_T, typename Base_T>
class Caster
  : public Abstract_Caster
{
public:
  Caster(Abstract_Caster * base_caster, Module type)
    : base_caster_(base_caster)
    , type_(type)
  {
  }

protected:
  virtual void * cast_to_base(void * derived, Module type) const
  {
    if(type.value() == type_.value())
    {
      Derived_T * d(static_cast<Derived_T *>(derived));
      return static_cast<Base_T *>(d);
    }
    else
    {
      if(base_caster_)
      {
        return base_caster_->cast_to_base(derived, type);
      }
      else
      {
        std::string s = "bad cast. No caster found for ";
        s += type_.name().str();
        throw std::runtime_error(s);
      }
    }
  }

private:
  Abstract_Caster * base_caster_;
  Module type_;
};

template<typename To_T, typename From_T>
class Implicit_Caster
  : public Abstract_Caster
{
public:
  Implicit_Caster(Abstract_Caster * base_caster, Module type)
    : base_caster_(base_caster)
    , type_(type)
  {
  }

protected:
  virtual void * cast_to_base(void * derived, Module type) const
  {
    if(type.value() == type_.value())
    {
      return new To_T( *static_cast<From_T*>(derived) );
    }
    else
    {
      if(base_caster_)
      {
        return base_caster_->cast_to_base(derived, type);
      }
      else
      {
        std::string s = "bad cast. No implicit caster found for ";
        s += type_.name().str();
        throw std::runtime_error(s);
      }
    }
  }

private:
  Abstract_Caster * base_caster_;
  Module type_;
};



} // detail

} // Rice

#endif //Rice__detail__Caster__hpp_
