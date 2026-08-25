#pragma once

#include "dndmud/Character.h"

#include <map>
#include <optional>
#include <string>

namespace dndmud {

/// 房间出口支持的四个基本方向。
enum class Direction {
    North,
    East,
    South,
    West
};

/// 保存一个房间的文字、出口和敌人信息。
struct Room {
    int id{};                           ///< 房间的唯一编号。
    std::string title;                  ///< 显示标题。
    std::string description;            ///< 场景描述。
    std::map<Direction, int> exits;      ///< 每个可走方向对应的下一个房间编号。
    std::optional<std::string> enemyId;  ///< 房间内敌人的编号；没有敌人时为空。
};

/// 管理所有房间之间的连接和当前敌人。
class World {
public:
    /// 创建包含三个房间和一只灰牙狼的演示地图。
    static World createDemo();

    /// 按编号查询房间；找不到时会报告范围错误。
    const Room& room(int roomId) const;
    /// 查询指定方向通往哪个房间；没有出口时返回空结果。
    std::optional<int> destination(int roomId, Direction direction) const;
    /// 返回房间内仍存活的敌人；无敌人或已死亡时返回空指针。
    Enemy* enemyInRoom(int roomId);
    const Enemy* enemyInRoom(int roomId) const;
    /// 访问当前演示版本中的唯一敌人，供保存和读取使用。
    Enemy& enemy() noexcept;
    const Enemy& enemy() const noexcept;

private:
    World(std::map<int, Room> rooms, Enemy enemy);

    std::map<int, Room> rooms_;
    Enemy enemy_;
};

/// 将 north、n 等英文方向文字转换为方向值。
std::optional<Direction> parseDirection(std::string_view text);
/// 返回方向的完整英文名称。
std::string directionName(Direction direction);

} // namespace dndmud
