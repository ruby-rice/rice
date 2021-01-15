#include "Object.hpp"
#include <algorithm>

inline Rice::Address_Registration_Guard::
Address_Registration_Guard(VALUE * address)
  : address_(address)
{
  registerExitHandler();
  rb_gc_register_address(address);
}

inline Rice::Address_Registration_Guard::
Address_Registration_Guard(Object * object)
  : address_(const_cast<VALUE *>(&object->value()))
{
  registerExitHandler();
  rb_gc_register_address(address_);
}

inline Rice::Address_Registration_Guard::
~Address_Registration_Guard()
{
  if (enabled)
    rb_gc_unregister_address(address_);
}

inline VALUE * Rice::Address_Registration_Guard::
address() const
{
  return address_;
}

inline void Rice::Address_Registration_Guard::
swap(Rice::Address_Registration_Guard & other)
{
  std::swap(address_, other.address_);
}

static void disable_all_guards(VALUE)
{
  Rice::Address_Registration_Guard::disable();
}

inline void Rice::Address_Registration_Guard::registerExitHandler()
{
  if (exit_handler_registered) return;
  rb_set_end_proc(&disable_all_guards, Qnil);
  exit_handler_registered = true;
}

inline void Rice::Address_Registration_Guard::disable()
{
  enabled = false;
}
