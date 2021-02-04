#include <any>
#include <string>

namespace Rice {

inline Return::Return(bool takeOwnership)
  : takeOwnership_(takeOwnership)
{
}

inline bool Return::takeOwnership()
{
  return this->takeOwnership_;
}

}  // Rice
