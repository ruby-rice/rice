#ifndef Rice__ruby_Critical_Guard__hpp_
#define Rice__ruby_Critical_Guard__hpp_

#include "detail/ruby_version_code.hpp"

#if RICE__RUBY_VERSION_CODE < 190

namespace Rice
{

//! A guard to prevent Ruby from switching threads.
/*! Sets rb_thread_critical to 1 upon construction and back to its
 *  original value upon destruction.  This prevents the scheduler from
 *  changing threads.  This does not work on YARV, however, which has a
 *  different threading model.
 */
class Critical_Guard
{
public:
  //! Prevent ruby from switching threads.
  /*! Prevent the ruby scheduler from switching threads by setting
   *  rb_thread_critical to 1.
   */
  Critical_Guard();

  //! Allow ruby to switch threads.
  /*! Allow the ruby scheduler to switch threads by setting
   *  rb_thread_critical to 0.
   */
  ~Critical_Guard();
};

}

#endif

#include "Critical_Guard.ipp"

#endif // Rice__ruby_Critical_Guard__hpp_

