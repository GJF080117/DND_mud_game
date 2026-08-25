#pragma once

#include "dndmud/Character.h"

#include <map>
#include <optional>
#include <string>

namespace dndmud {

enum class Direction {
    North,
    East,
    South,
    West
};

struct Room {
    int id{};
    std::string title;
    std::string description;
    std::map<Direction, int> exits;
    std::optional<std::string> enemyId;
};

class World {
public:
    static World createDemo();

    const Room& room(int roomId) const;
    std::optional<int> destination(int roomId, Direction direction) const;
    Enemy* enemyInRoom(int roomId);
    const Enemy* enemyInRoom(int roomId) const;
    Enemy& enemy() noexcept;
    const Enemy& enemy() const noexcept;

private:
    World(std::map<int, Room> rooms, Enemy enemy);

    std::map<int, Room> rooms_;
    Enemy enemy_;
};

std::optional<Direction> parseDirection(std::string_view text);
std::string directionName(Direction direction);

} // namespace dndmud
