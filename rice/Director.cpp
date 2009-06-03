#include "Director.hpp"

#include "detail/env.hpp"

namespace Rice {

  Director::Director(Object self) {
    self_ = self;
  }

  bool Director::callIsFromRuby(const char* methodName) const {
    return (getSelf().value() == ruby_frame->self) && ( rb_id2name(ruby_frame->orig_func) != methodName );
  }

  void Director::raisePureVirtual() const {
    rb_raise(rb_eNotImpError, "Cannot call super() into a pure-virtual C++ method");
  }

}
