#include <any>
#include <string>

namespace Rice
{
  inline ReturnInfo& ReturnInfo::takeOwnership()
  {
    this->isOwner_ = true;
    return *this;
  }

  inline bool ReturnInfo::isOwner()
  {
    return this->isOwner_;
  }

  inline ReturnInfo& ReturnInfo::isValue()
  {
    this->isValue_ = true;
    return *this;
  }

  inline bool ReturnInfo::getIsValue()
  {
    return this->isValue_;
  }
}  // Rice
