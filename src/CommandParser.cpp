#include "dndmud/CommandParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace dndmud {
namespace {

std::string normalize(std::string_view input) {
    std::string value(input);
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    value = value.substr(first, last - first + 1);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

} // namespace

ParsedCommand CommandParser::parse(std::string_view input) const {
    const std::string normalized = normalize(input);
    if (normalized.empty()) {
        return {};
    }

    std::istringstream stream(normalized);
    std::string verb;
    std::string argument;
    stream >> verb;
    std::getline(stream >> std::ws, argument);

    if (verb == "help" || verb == "?") return {CommandType::Help, argument};
    if (verb == "look" || verb == "l") return {CommandType::Look, argument};
    if (verb == "status") return {CommandType::Status, argument};
    if (verb == "inventory" || verb == "i") return {CommandType::Inventory, argument};
    if (verb == "attack" || verb == "a") return {CommandType::Attack, argument};
    if (verb == "use") return {CommandType::Use, argument};
    if (verb == "save") return {CommandType::Save, argument};
    if (verb == "load") return {CommandType::Load, argument};
    if (verb == "quit" || verb == "q") return {CommandType::Quit, argument};
    if (verb == "go") return {CommandType::Move, argument};
    if (verb == "north" || verb == "n" || verb == "east" || verb == "e"
        || verb == "south" || verb == "s" || verb == "west" || verb == "w") {
        return {CommandType::Move, verb};
    }
    return {CommandType::Unknown, normalized};
}

} // namespace dndmud
