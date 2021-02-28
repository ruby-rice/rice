#include <any>
#include <string>

namespace Rice {

inline Return& Return::takeOwnership(bool value)
{
  this->isOwner_ = value;
  return *this;
}

inline bool Return::isOwner()
{
  return this->isOwner_;
}

}  // Rice
