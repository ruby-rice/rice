#include <any>
#include <string>

namespace Rice
{
  inline Return& Return::takeOwnership()
  {
    this->isOwner_ = true;
    return *this;
  }

  inline bool Return::isOwner()
  {
    return this->isOwner_;
  }

  inline Return& Return::isValue()
  {
    this->isValue_ = true;
    return *this;
  }

  inline bool Return::getIsValue()
  {
    return this->isValue_;
  }
}  // Rice
