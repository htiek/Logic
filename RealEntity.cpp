#include "RealEntity.h"
#include <stdexcept>
#include <vector>
#include <sstream>
using namespace std;

bool Sunny(Entity e) {
    return e->isSunny;
}
bool Rainy(Entity e) {
    return e->isRainy;
}
bool Cloudy(Entity e) {
    return e->isCloudy;
}
bool IsNextDay(Entity x, Entity y) {
    return x->futures.count(y.get());
}

FOL::BuildContext entityBuildContext(Entity today) {
    return {
        {
            { "Today", today }
        },
        {
            { "Sunny", { 1, [](const vector<Entity>& e) { return Sunny(e[0]); } } },
            { "Rainy",    { 1, [](const vector<Entity>& e) { return Rainy(e[0]); } } },
            { "Cloudy",  { 1, [](const vector<Entity>& e) { return Cloudy(e[0]); } } },
            { "IsNextDay",  { 2, [](const vector<Entity>& e) { return IsNextDay(e[0], e[1]); } } },
        },
        {} /* Functions: None */
    };
}

