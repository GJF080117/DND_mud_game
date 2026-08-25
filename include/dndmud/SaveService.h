#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace dndmud {

struct GameSnapshot {
    std::string playerName;
    int playerHealth{};
    int roomId{};
    int potionCount{};
    int enemyHealth{};
    bool won{};
};

class SaveService {
public:
    static bool save(
        const GameSnapshot& snapshot,
        const std::filesystem::path& path,
        std::string& error);

    static std::optional<GameSnapshot> load(
        const std::filesystem::path& path,
        std::string& error);
};

} // namespace dndmud
