#include "Symbol.hpp"

Rice::Identifier::
Identifier(Symbol const & symbol)
  : id_(SYM2ID(symbol.value()))
{
}

