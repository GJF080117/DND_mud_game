#pragma once

#include <string>
#include <string_view>

namespace dndmud {

/// 游戏能够识别的命令类别。
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

/// 保存识别出的命令种类和后面的参数。
struct ParsedCommand {
    CommandType type{CommandType::Unknown};
    std::string argument;
};

/// 识别用户输入的文字，但不直接执行游戏操作。
class CommandParser {
public:
    /// 去除首尾空格、忽略英文字母大小写并识别简写命令。
    ParsedCommand parse(std::string_view input) const;
};

} // namespace dndmud
