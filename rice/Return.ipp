#include <any>

namespace Rice
{
  inline Return::Return(): Arg("Return")
  {
  }

  inline Return& Return::keepAlive()
  {
    Arg::keepAlive();
    return *this;
  }

  inline Return& Return::setValue()
  {
    Arg::setValue();
    return *this;
  }

  inline Return& Return::setOpaque()
  {
    Arg::setOpaque();
    return *this;
  }

  inline Return& Return::takeOwnership()
  {
    Arg::takeOwnership();
    return *this;
  }

  inline Return& Return::setBuffer()
  {
    Arg::setBuffer();
    return *this;
  }
}  // Rice
