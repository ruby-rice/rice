#include "Director.hpp"

#include "env.h"

#include <iostream>
using namespace std;

namespace Rice {

  Director::Director(Object self) {
    self_ = self;
  }

  bool Director::callIsFromRuby(const char* methodName) {
    return (getSelf().value() == ruby_frame->self) && ( rb_id2name(ruby_frame->orig_func) != methodName );
  }

  void Director::raisePureVirtual() {
    rb_raise(rb_eNotImpError, "Cannot call super() into a pure-virtual C++ method");
  }

}
