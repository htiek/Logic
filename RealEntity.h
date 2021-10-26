#ifndef RealEntity_Included
#define RealEntity_Included

#include "Entity.h"
#include "FOLExpressionBuilder.h"
#include <memory>
#include <string>

struct RealEntity {
  std::string name;
  std::set<RealEntity *> futures;

  bool isRainy  = false;
  bool isSunny  = false;
  bool isCloudy = false;
  
  RealEntity(const std::string& name, std::set<RealEntity *> futures = {}) : name(name), futures(futures) {}
};

FOL::BuildContext entityBuildContext(Entity today);

#endif
