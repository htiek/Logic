#ifndef Entity_Included
#define Entity_Included

#include <memory>
#include <set>

using Entity = std::shared_ptr<struct RealEntity>;
using World  = std::set<Entity>;

bool Sunny(Entity e);
bool Rainy(Entity e);
bool Cloudy(Entity e);
bool IsNextDay(Entity now, Entity later);

#endif
