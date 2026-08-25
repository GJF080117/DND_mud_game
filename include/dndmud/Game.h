#pragma once

#include "dndmud/Character.h"
#include "dndmud/CombatSystem.h"
#include "dndmud/CommandParser.h"
#include "dndmud/Random.h"
#include "dndmud/SaveService.h"
#include "dndmud/World.h"

#include <filesystem>
#include <iosfwd>
#include <string>

namespace dndmud {

class Game {
public:
    explicit Game(IRandomSource& randomSource);

    void newGame(std::string playerName);
    bool load(const std::filesystem::path& savePath, std::ostream& output);
    bool save(const std::filesystem::path& savePath, std::ostream& output) const;
    void run(
        std::istream& input,
        std::ostream& output,
        const std::filesystem::path& savePath);

    bool executeLine(
        const std::string& line,
        std::ostream& output,
        const std::filesystem::path& savePath);

    const Player& player() const noexcept;
    const World& world() const noexcept;

private:
    void showHelp(std::ostream& output) const;
    void showRoom(std::ostream& output) const;
    void showStatus(std::ostream& output) const;
    void showInventory(std::ostream& output) const;
    void move(const std::string& argument, std::ostream& output);
    void fight(std::ostream& output);
    void useItem(const std::string& argument, std::ostream& output);
    GameSnapshot snapshot() const;
    void restore(const GameSnapshot& snapshot);

    World world_;
    Player player_;
    CombatSystem combatSystem_;
    CommandParser commandParser_;
};

} // namespace dndmud
