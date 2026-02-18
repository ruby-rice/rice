#ifndef Rice__Director__ipp_
#define Rice__Director__ipp_

namespace Rice
{
  inline Director::Director(Object self) : self_(self)
  {
  }

  inline void Director::raisePureVirtual() const
  {
    rb_raise(rb_eNotImpError, "Cannot call super() into a pure-virtual C++ method");
  }

  inline Object Director::getSelf() const
  {
    return self_;
  }
}
#endif // Rice__Director__ipp_
