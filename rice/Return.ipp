#include <any>
#include <string>

namespace Rice {

inline Return& Return::takeOwnership()
{
  this->isOwner_ = true;
  return *this;
}

inline bool Return::isOwner()
{
  return this->isOwner_;
}

}  // Rice
