#include "Director.hpp"

#include "detail/ruby.hpp"

#ifdef RUBY_VM
/* YARV */
#include "detail/cfp.hpp"
#else
/* pre-YARV */
#include "detail/env.hpp"
#endif

namespace Rice {

  Director::Director(Object self) {
    self_ = self;
  }

  bool Director::callIsFromRuby(const char* methodName) const {
#   ifdef RUBY_VM
    VALUE * cfp = Rice::detail::cfp();
    return
      (getSelf().value() == Rice::detail::cfp_self(cfp)) && 
      (rb_id2name(rb_frame_callee()) != methodName);
#   else
    return
      (getSelf().value() == ruby_frame->self) &&
      (rb_id2name(ruby_frame->orig_func) != methodName);
#   endif
  }

  void Director::raisePureVirtual() const {
    rb_raise(rb_eNotImpError, "Cannot call super() into a pure-virtual C++ method");
  }

}
