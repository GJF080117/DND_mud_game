#include "dndmud/World.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace dndmud {
namespace {

// 方向命令只使用英文，因此这里只处理英文字母大小写。
std::string asciiLower(std::string_view text) {
    std::string value(text);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

} // namespace

World::World(std::map<int, Room> rooms, Enemy enemy)
    : rooms_(std::move(rooms)), enemy_(std::move(enemy)) {}

World World::createDemo() {
    // 每个房间都有固定编号，出口只记录下一个房间的编号。
    std::map<int, Room> rooms;
    rooms.emplace(0, Room{
        0,
        "灰港码头",
        "潮水拍打黑色木桩。北面的旧城门半掩在雾中。",
        {{Direction::North, 1}},
        std::nullopt});
    rooms.emplace(1, Room{
        1,
        "旧城门",
        "断裂的旗帜悬在城墙上。东面废墟里传来低沉的喘息。",
        {{Direction::East, 2}, {Direction::South, 0}},
        std::nullopt});
    rooms.emplace(2, Room{
        2,
        "哨塔废墟",
        "碎石中央散落着商队的货箱，一只灰牙狼守在这里。",
        {{Direction::West, 1}},
        std::string("grey_wolf")});

    // 当前演示版本只有一个敌人，房间通过敌人编号找到它。
    Enemy wolf("grey_wolf", "灰牙狼", CombatStats{10, 11, 2, 1, 4});
    return World(std::move(rooms), std::move(wolf));
}

const Room& World::room(int roomId) const {
    return rooms_.at(roomId);
}

std::optional<int> World::destination(int roomId, Direction direction) const {
    const auto& exits = room(roomId).exits;
    const auto found = exits.find(direction);
    if (found == exits.end()) {
        return std::nullopt;
    }
    return found->second;
}

Enemy* World::enemyInRoom(int roomId) {
    const auto& enemyId = room(roomId).enemyId;
    // 已死亡敌人不再作为可交互目标返回。
    if (enemyId && *enemyId == enemy_.id() && enemy_.isAlive()) {
        return &enemy_;
    }
    return nullptr;
}

const Enemy* World::enemyInRoom(int roomId) const {
    const auto& enemyId = room(roomId).enemyId;
    if (enemyId && *enemyId == enemy_.id() && enemy_.isAlive()) {
        return &enemy_;
    }
    return nullptr;
}

Enemy& World::enemy() noexcept { return enemy_; }
const Enemy& World::enemy() const noexcept { return enemy_; }

std::optional<Direction> parseDirection(std::string_view text) {
    const std::string value = asciiLower(text);
    if (value == "north" || value == "n") return Direction::North;
    if (value == "east" || value == "e") return Direction::East;
    if (value == "south" || value == "s") return Direction::South;
    if (value == "west" || value == "w") return Direction::West;
    return std::nullopt;
}

std::string directionName(Direction direction) {
    switch (direction) {
    case Direction::North: return "north";
    case Direction::East: return "east";
    case Direction::South: return "south";
    case Direction::West: return "west";
    }
    throw std::logic_error("unknown direction");
}

} // namespace dndmud
