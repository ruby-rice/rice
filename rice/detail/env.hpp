#ifndef Rice__detail__env__hpp_
#define Rice__detail__env__hpp_

/**
 * Helper header for the env.h ruby include file, which does
 * not have extern "C"
 */

// TODO: Won't work on ruby 1.9

extern "C" {
  #include "env.h"
}

#endif
