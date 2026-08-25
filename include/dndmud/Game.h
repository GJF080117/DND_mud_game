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

/// 组织命令、地图、战斗、存档和文字显示，是游戏流程的总入口。
class Game {
public:
    /// 使用外部提供的随机数工具；该工具在游戏运行期间必须存在。
    explicit Game(IRandomSource& randomSource);

    /// 重置为新游戏；空名称会替换为默认名称。
    void newGame(std::string playerName);
    /// 从路径读取存档，并把结果说明写入 output。
    bool load(const std::filesystem::path& savePath, std::ostream& output);
    /// 保存当前状态，并把结果说明写入 output。
    bool save(const std::filesystem::path& savePath, std::ostream& output) const;
    /// 运行命令循环，直到退出、玩家死亡或输入结束。
    void run(
        std::istream& input,
        std::ostream& output,
        const std::filesystem::path& savePath);

    /// 执行一条命令；返回 false 表示应该结束当前游戏。
    bool executeLine(
        const std::string& line,
        std::ostream& output,
        const std::filesystem::path& savePath);

    /// 只读访问玩家和地图，供显示或测试使用。
    const Player& player() const noexcept;
    const World& world() const noexcept;

private:
    /// 以下每个函数只处理一种显示或操作，由 executeLine 统一选择。
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
