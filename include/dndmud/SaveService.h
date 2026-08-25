#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace dndmud {

/// 集中保存写入文件所需的游戏数据。
struct GameSnapshot {
    std::string playerName; ///< 玩家名称。
    int playerHealth{};     ///< 玩家当前生命值。
    int roomId{};           ///< 玩家当前房间 ID。
    int potionCount{};      ///< 剩余药水数量。
    int enemyHealth{};      ///< 当前演示版本中唯一敌人的生命值。
    bool won{};             ///< 主线是否完成。
};

/// 负责写入和读取文本存档，并在保存失败时保护旧文件。
class SaveService {
public:
    /// 保存游戏数据；失败时返回 false，并在 error 中说明原因。
    static bool save(
        const GameSnapshot& snapshot,
        const std::filesystem::path& path,
        std::string& error);

    /// 读取并检查游戏数据；失败时返回空结果，并在 error 中说明原因。
    static std::optional<GameSnapshot> load(
        const std::filesystem::path& path,
        std::string& error);
};

} // namespace dndmud
