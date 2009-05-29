#include "Director.hpp"

#include "env.h"

namespace Rice {

  bool Director::callIsFromRuby(const char* methodName) {
    return getSelf().value() == ruby_frame->self;
  }

}
