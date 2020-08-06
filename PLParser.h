#ifndef PLParser_Included
#define PLParser_Included

#include "Scanner.h"
#include <queue>
#include <memory>
#include "PLExpression.h"


std::shared_ptr<PL::Expression> parsePL(std::queue<Token>& q);
std::shared_ptr<PL::Expression> parsePL(std::queue<Token>&& q);

#endif
