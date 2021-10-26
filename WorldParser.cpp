#include "WorldParser.h"
#include "RealEntity.h"
#include "StrUtils/StrUtils.h"
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

namespace {
    struct ParsedLine {
        string type;
        vector<string> args;
    };

    /* Given a line of text from a file, parses that line into a more structured
     * format. If the line can't be parsed, reports an error.
     */
    ParsedLine parse(const string& line) {
        ParsedLine result;

        /* The expected format is
         *
         *    SomeFunctionSymbol(ArgList)
         *
         * So let's grab everything up to that first parenthesis.
         */
        auto openIndex = line.find('(');
        if (openIndex == string::npos) throw runtime_error("Couldn't find an open parenthesis in input line.");
        result.type = Utilities::trim(line.substr(0, openIndex));

        /* Find the close parenthesis that matches this open parenthesis. Make sure that it's
         * at the very end.
         */
        auto closeIndex = line.find(')', openIndex);
        if (closeIndex != line.size() - 1) throw runtime_error("Extra tokens found after close parenthesis.");

        /* Split the inner part of the string to get the arguments. */
        result.args = Utilities::stringSplit(string(line.begin() + openIndex + 1, line.end() - 1), ',');

        /* Trim all leading and trailing whitespace from each argument. */
        for (auto& arg: result.args) {
            arg = Utilities::trim(arg);
            if (arg.empty()) throw runtime_error("Empty argument found in parameter list.");
        }
        return result;
    }

    /* Creates an entity with the given name. */
    void declareEntity(const vector<string>& args,
                       World& world,
                       map<string, Entity>& byName) {
        /* Confirm that there's a single argument. */
        if (args.size() != 1) throw runtime_error("The Day() predicate requires exactly one argument.");

        /* If this entity doesn't already exist, create it. */
        if (!byName.count(args[0])) {
            byName[args[0]] = make_shared<RealEntity>(args[0]);
            world.insert(byName[args[0]]);
        }
    }

    /* Adds the given attribute to the specified entity. */
    void updateEntity(const vector<string>& args,
                      map<string, Entity>& byName,
                      bool RealEntity::* type,
                      const string& predicate) {
        /* Confirm that there's a single argument. */
        if (args.size() != 1) throw runtime_error("The " + predicate + " predicate requires exactly one argument.");

        /* Update its properties. */
        byName.at(args[0]).get()->*type = true;
    }

    /* Given a person's name, either returns that person or reports that they don't yet
     * exist.
     */
    RealEntity* entityNamed(const string& name, map<string, Entity>& byName) {
        auto result = byName.find(name);
        if (result == byName.end()) throw runtime_error("Entity doesn't exist (yet?): " + name);
        return result->second.get();
    }

    /* Given a command to say day is a future of the other, records this or reports
     * an error if this isn't possible.
     */
    void processFuture(const vector<string>& args, map<string, Entity>& byName) {
        /* Confirm that there's two arguments. */
        if (args.size() != 2) throw runtime_error("The Loves predicate requires exactly two arguments.");

        entityNamed(args[0], byName)->futures.insert(entityNamed(args[1], byName));
    }
}

/* Given a stream that contains a definition of a world, parses that world
 * into a World object. Does what might be best described as pedantic
 * error-checking.
 */
World parseWorld(istream& source) {
    World result;
    map<string, Entity> byName; // Name -> Entity

    for (string line; getline(source, line); ) {
        /* If there's a pound symbol, delete everything after it. */
        line.erase(find(line.begin(), line.end(), '#'), line.end());

        /* Trim our line to remove any stray whitespace. */
        line = Utilities::trim(line);

        /* If the line is empty, it's either blank or it was a comment
         * and we can safely skip it.
         */
        if (line.empty()) continue;

        /* Determine what kind of line we're looking at. */
        ParsedLine parsedLine = parse(line);

        if (parsedLine.type == "Day") {
            declareEntity(parsedLine.args, result, byName);
        } else if (parsedLine.type == "Sunny") {
            updateEntity(parsedLine.args, byName, &RealEntity::isSunny, parsedLine.type);
        } else if (parsedLine.type == "Rainy") {
            updateEntity(parsedLine.args, byName, &RealEntity::isRainy, parsedLine.type);
        } else if (parsedLine.type == "Cloudy") {
            updateEntity(parsedLine.args, byName, &RealEntity::isCloudy, parsedLine.type);
        } else if (parsedLine.type == "IsNextDay") {
            processFuture(parsedLine.args, byName);
        } else {
            throw runtime_error("Unknown statement type: [" + line + "]");
        }
    }

    return result;
}

/* Serializes a world so that it can be read back in later. */
ostream& operator<< (ostream& out, const World& world) {
    ostringstream result;

    /* Stash all entity declarations in a vector so we can sort them. */
    vector<string> entities;

    /* Write entity declarations. */
    for (const auto& entry: world) {
        entities.push_back("Day(" + entry->name + ")");
    }
    sort(entities.begin(), entities.end());
    copy(entities.begin(), entities.end(), ostream_iterator<string>(result, "\n"));

    /* Write entity properties. */

    vector<string> properties;
    for (const auto& entry: world) {
        if (entry->isCloudy) properties.push_back("Cloudy(" + entry->name + ")");
        if (entry->isRainy) properties.push_back("Rainy(" + entry->name + ")");
        if (entry->isSunny) properties.push_back("Sunny(" + entry->name + ")");
    }
    sort(properties.begin(), properties.end());
    copy(properties.begin(), properties.end(), ostream_iterator<string>(result, "\n"));

    /* Write links. */
    vector<string> links;
    for (const auto& today: world) {
        for (const auto& tomorrow: today->futures) {
            links.push_back("IsNextDay(" + today->name + ", " + tomorrow->name + ")");
        }
    }

    sort(links.begin(), links.end());
    copy(links.begin(), links.end(), ostream_iterator<string>(result, "\n"));

    return out << result.str();
}
