#include "Director.hpp"

namespace Rice {

  Director::Director(Object self) {
    self_ = self;
  }

  void Director::raisePureVirtual() const {
    rb_raise(rb_eNotImpError, "Cannot call super() into a pure-virtual C++ method");
  }

}
