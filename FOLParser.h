#ifndef FOLParser_Included
#define FOLParser_Included

#include "Scanner.h"
#include <queue>
#include <memory>
#include "FOLAST.h"


std::shared_ptr<FOL::ASTNode> parseFOL(std::queue<Token>& q);
std::shared_ptr<FOL::ASTNode> parseFOL(std::queue<Token>&& q);

#endif
