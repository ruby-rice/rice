#ifndef Rice__detail__rubysig__hpp_
#define Rice__detail__rubysig__hpp_

#include "ruby_version_code.hpp"

/**
 * Helper header for the rubysig.h ruby include file, which does
 * not have extern "C"
 */

#if RICE__RUBY_VERSION_CODE < 190

extern "C" {
  #include "rubysig.h"
}

#endif

#endif
