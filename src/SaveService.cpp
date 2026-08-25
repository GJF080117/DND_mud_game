#include "dndmud/SaveService.h"

#include <fstream>
#include <iomanip>
#include <system_error>

namespace dndmud {
namespace {

// 修改存档内容或顺序时要更换版本号，避免用错误方式读取旧存档。
constexpr const char* SaveHeader = "DNDMUD_SAVE_V1";

// 保存和读取前都检查数值范围，防止错误数据进入游戏。
bool validSnapshot(const GameSnapshot& snapshot) {
    return !snapshot.playerName.empty()
        && snapshot.playerName.size() <= 40
        && snapshot.playerHealth >= 0
        && snapshot.playerHealth <= 24
        && snapshot.roomId >= 0
        && snapshot.roomId <= 2
        && snapshot.potionCount >= 0
        && snapshot.potionCount <= 20
        && snapshot.enemyHealth >= 0
        && snapshot.enemyHealth <= 10;
}

} // namespace

bool SaveService::save(
    const GameSnapshot& snapshot,
    const std::filesystem::path& path,
    std::string& error) {
    if (!validSnapshot(snapshot)) {
        error = "存档数据不在允许范围内。";
        return false;
    }

    // 先完整写入临时文件，避免写入中断直接破坏正式存档。
    const std::filesystem::path temporary = path.string() + ".tmp";
    std::ofstream output(temporary, std::ios::trunc);
    if (!output) {
        error = "无法创建临时存档文件。";
        return false;
    }

    output << SaveHeader << '\n'
           << "player_name " << std::quoted(snapshot.playerName) << '\n'
           << "player_health " << snapshot.playerHealth << '\n'
           << "room_id " << snapshot.roomId << '\n'
           << "potion_count " << snapshot.potionCount << '\n'
           << "enemy_health " << snapshot.enemyHealth << '\n'
           << "won " << (snapshot.won ? 1 : 0) << '\n';
    output.close();
    if (!output) {
        error = "写入存档时发生错误。";
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        return false;
    }

    std::error_code filesystemError;
    const std::filesystem::path backup = path.string() + ".bak";
    const bool hadPreviousSave = std::filesystem::exists(path, filesystemError);
    if (filesystemError) {
        error = "无法检查旧存档：" + filesystemError.message();
        std::filesystem::remove(temporary, filesystemError);
        return false;
    }

    if (hadPreviousSave) {
        // 保留旧版本，直到新临时文件成功替换为正式文件。
        std::filesystem::remove(backup, filesystemError);
        filesystemError.clear();
        std::filesystem::rename(path, backup, filesystemError);
        if (filesystemError) {
            error = "无法备份旧存档：" + filesystemError.message();
            std::filesystem::remove(temporary, filesystemError);
            return false;
        }
    }

    filesystemError.clear();
    std::filesystem::rename(temporary, path, filesystemError);
    if (filesystemError) {
        error = "无法替换正式存档：" + filesystemError.message();
        std::filesystem::remove(temporary, filesystemError);
        if (hadPreviousSave) {
            // 替换失败时尽力恢复旧存档；原始错误仍返回给调用方。
            filesystemError.clear();
            std::filesystem::rename(backup, path, filesystemError);
        }
        return false;
    }

    if (hadPreviousSave) {
        std::filesystem::remove(backup, filesystemError);
    }

    error.clear();
    return true;
}

std::optional<GameSnapshot> SaveService::load(
    const std::filesystem::path& path,
    std::string& error) {
    std::ifstream input(path);
    if (!input) {
        error = "没有找到可读取的存档。";
        return std::nullopt;
    }

    std::string header;
    std::string key;
    int wonValue = 0;
    GameSnapshot snapshot;

    // 当前版本按固定顺序读取；缺少字段、字段改名或版本不同都算读取失败。
    if (!(input >> header) || header != SaveHeader
        || !(input >> key) || key != "player_name" || !(input >> std::quoted(snapshot.playerName))
        || !(input >> key) || key != "player_health" || !(input >> snapshot.playerHealth)
        || !(input >> key) || key != "room_id" || !(input >> snapshot.roomId)
        || !(input >> key) || key != "potion_count" || !(input >> snapshot.potionCount)
        || !(input >> key) || key != "enemy_health" || !(input >> snapshot.enemyHealth)
        || !(input >> key) || key != "won" || !(input >> wonValue)) {
        error = "存档格式损坏或版本不匹配。";
        return std::nullopt;
    }

    snapshot.won = wonValue == 1;
    std::string trailing;
    // 文件末尾出现多余内容或数值超出范围时，拒绝使用整个存档。
    if ((wonValue != 0 && wonValue != 1) || (input >> trailing) || !validSnapshot(snapshot)) {
        error = "存档字段包含非法值。";
        return std::nullopt;
    }

    error.clear();
    return snapshot;
}

} // namespace dndmud
