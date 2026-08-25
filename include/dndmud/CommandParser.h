#pragma once

#include <string>
#include <string_view>

namespace dndmud {

enum class CommandType {
    Help,
    Look,
    Move,
    Status,
    Inventory,
    Attack,
    Use,
    Save,
    Load,
    Quit,
    Unknown
};

struct ParsedCommand {
    CommandType type{CommandType::Unknown};
    std::string argument;
};

class CommandParser {
public:
    ParsedCommand parse(std::string_view input) const;
};

} // namespace dndmud
