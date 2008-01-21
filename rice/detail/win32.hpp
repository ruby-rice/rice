#ifndef Rice__detail__win32__hpp_
#define Rice__detail__win32__hpp_

/*! \file
 * \brief Undefine all the evil macros that are defined in win32.h
 */

// None of these macros are used in Ruby header files
#undef bind
#undef read
#undef write
#undef send
#undef recv
#undef times

#endif // Rice__detail__win32__hpp_
