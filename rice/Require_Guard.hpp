#ifndef Rice__Require_Guard__hpp_
#define Rice__Require_Guard__hpp_

/*! \def RICE_REQUIRE_GUARD
 *  \brief Put this inside your Init_module function to keep it from
 *         being required more than once with the same name (if you use
 *         Rice, and your module is initialized more than once, an
 *         exception will be thrown).
 */
#define RICE_REQUIRE_GUARD \
  static bool Rice__module_initialized = false; \
  if(Rice__module_initialized) \
  { \
    return; \
  } \
  /* TODO: If module initialization fails, it's not possible to */ \
  /* retry */ \
  Rice__module_initialized = true

#endif // Rice__Require_Guard__hpp_

