#include "dndmud/SaveService.h"

#include <fstream>
#include <iomanip>
#include <system_error>

namespace dndmud {
namespace {

constexpr const char* SaveHeaderV1 = "DNDMUD_COMPLETE_SAVE_V1";
constexpr const char* SaveHeaderV2 = "DNDMUD_COMPLETE_SAVE_V2";

bool basicValuesAreValid(const SavedGame& game) {
    const int classValue = static_cast<int>(game.player.playerClass);
    return !game.player.name.empty()
        && game.player.name.size() <= 80
        && classValue >= 1 && classValue <= 3
        && game.player.level >= 1 && game.player.level <= 20
        && game.player.experience >= 0
        && game.player.maxHealth > 0 && game.player.maxHealth <= 500
        && game.player.health >= 0 && game.player.health <= game.player.maxHealth
        && game.player.gold >= 0 && game.player.gold <= 1000000
        && game.player.skillUses >= 0 && game.player.skillUses <= 2
        && game.currentRoom >= 0
        && game.mainQuestStage >= 0 && game.mainQuestStage <= 6
        && game.sideQuestStage >= 0 && game.sideQuestStage <= 2
        && game.coastQuestStage >= 0 && game.coastQuestStage <= 2
        && game.player.inventory.size() <= 200
        && game.defeatedEnemies.size() <= 100;
}

} // namespace

bool SaveService::save(
    const SavedGame& game,
    const std::filesystem::path& path,
    std::string& error) {
    if (!basicValuesAreValid(game)) {
        error = "游戏数据超出允许范围。";
        return false;
    }

    std::error_code fileError;
    std::filesystem::create_directories(path.parent_path(), fileError);
    if (fileError) {
        error = "无法创建存档目录：" + fileError.message();
        return false;
    }

    auto temporary = path;
    temporary += ".tmp";
    auto backup = path;
    backup += ".bak";
    std::ofstream output(temporary, std::ios::trunc);
    if (!output) {
        error = "无法创建临时存档。";
        return false;
    }

    output << SaveHeaderV2 << '\n'
           << "name " << std::quoted(game.player.name) << '\n'
           << "class " << static_cast<int>(game.player.playerClass) << '\n'
           << "level " << game.player.level << '\n'
           << "experience " << game.player.experience << '\n'
           << "health " << game.player.health << '\n'
           << "max_health " << game.player.maxHealth << '\n'
           << "base_defense " << game.player.baseDefense << '\n'
           << "base_attack " << game.player.baseAttack << '\n'
           << "damage_min " << game.player.damageMinimum << '\n'
           << "damage_max " << game.player.damageMaximum << '\n'
           << "gold " << game.player.gold << '\n'
           << "skill_uses " << game.player.skillUses << '\n'
           << "weapon " << std::quoted(game.player.equippedWeapon) << '\n'
           << "armor " << std::quoted(game.player.equippedArmor) << '\n'
           << "room " << game.currentRoom << '\n'
           << "main_quest " << game.mainQuestStage << '\n'
           << "side_quest " << game.sideQuestStage << '\n'
           << "coast_quest " << game.coastQuestStage << '\n'
           << "finished " << (game.finished ? 1 : 0) << '\n'
           << "inventory_count " << game.player.inventory.size() << '\n';
    for (const auto& itemId : game.player.inventory) {
        output << "inventory_item " << std::quoted(itemId) << '\n';
    }
    output << "defeated_count " << game.defeatedEnemies.size() << '\n';
    for (const auto& enemyId : game.defeatedEnemies) {
        output << "defeated_enemy " << std::quoted(enemyId) << '\n';
    }
    output.close();
    if (!output) {
        error = "写入存档时发生错误。";
        std::filesystem::remove(temporary, fileError);
        return false;
    }

    const bool hadOldSave = std::filesystem::exists(path, fileError);
    if (fileError) {
        error = "无法检查旧存档：" + fileError.message();
        std::filesystem::remove(temporary, fileError);
        return false;
    }

    if (hadOldSave) {
        std::filesystem::remove(backup, fileError);
        fileError.clear();
        std::filesystem::rename(path, backup, fileError);
        if (fileError) {
            error = "无法备份旧存档：" + fileError.message();
            std::filesystem::remove(temporary, fileError);
            return false;
        }
    }

    fileError.clear();
    std::filesystem::rename(temporary, path, fileError);
    if (fileError) {
        error = "无法替换正式存档：" + fileError.message();
        std::filesystem::remove(temporary, fileError);
        if (hadOldSave) {
            fileError.clear();
            std::filesystem::rename(backup, path, fileError);
        }
        return false;
    }

    if (hadOldSave) std::filesystem::remove(backup, fileError);
    error.clear();
    return true;
}

std::optional<SavedGame> SaveService::load(
    const std::filesystem::path& path,
    std::string& error) {
    std::ifstream input(path);
    if (!input) {
        error = "没有找到该存档。";
        return std::nullopt;
    }

    SavedGame game;
    std::string header;
    std::string key;
    int classValue = 0;
    int finishedValue = 0;

    auto readNumber = [&](const char* expected, int& value) {
        return (input >> key) && key == expected && (input >> value);
    };
    auto readText = [&](const char* expected, std::string& value) {
        return (input >> key) && key == expected && (input >> std::quoted(value));
    };

    if (!(input >> header) || (header != SaveHeaderV1 && header != SaveHeaderV2)) {
        error = "存档格式损坏或版本不匹配。";
        return std::nullopt;
    }
    const bool isVersionOne = header == SaveHeaderV1;

    if (!readText("name", game.player.name)
        || !readNumber("class", classValue)
        || !readNumber("level", game.player.level)
        || !readNumber("experience", game.player.experience)
        || !readNumber("health", game.player.health)
        || !readNumber("max_health", game.player.maxHealth)
        || !readNumber("base_defense", game.player.baseDefense)
        || !readNumber("base_attack", game.player.baseAttack)
        || !readNumber("damage_min", game.player.damageMinimum)
        || !readNumber("damage_max", game.player.damageMaximum)
        || !readNumber("gold", game.player.gold)
        || !readNumber("skill_uses", game.player.skillUses)
        || !readText("weapon", game.player.equippedWeapon)
        || !readText("armor", game.player.equippedArmor)
        || !readNumber("room", game.currentRoom)
        || !readNumber("main_quest", game.mainQuestStage)
        || !readNumber("side_quest", game.sideQuestStage)) {
        error = "存档格式损坏或版本不匹配。";
        return std::nullopt;
    }

    if (!isVersionOne && !readNumber("coast_quest", game.coastQuestStage)) {
        error = "存档中的海岸任务记录不完整。";
        return std::nullopt;
    }
    if (!readNumber("finished", finishedValue)) {
        error = "存档中的结局记录不完整。";
        return std::nullopt;
    }

    game.player.playerClass = static_cast<PlayerClass>(classValue);
    game.finished = finishedValue == 1;
    if (isVersionOne && game.finished && game.mainQuestStage == 3) {
        game.finished = false;
    }

    int inventoryCount = 0;
    if (!readNumber("inventory_count", inventoryCount) || inventoryCount < 0 || inventoryCount > 200) {
        error = "存档中的背包数量不正确。";
        return std::nullopt;
    }
    for (int index = 0; index < inventoryCount; ++index) {
        std::string itemId;
        if (!readText("inventory_item", itemId)) {
            error = "存档中的背包内容不完整。";
            return std::nullopt;
        }
        game.player.inventory.push_back(std::move(itemId));
    }

    int defeatedCount = 0;
    if (!readNumber("defeated_count", defeatedCount) || defeatedCount < 0 || defeatedCount > 100) {
        error = "存档中的敌人记录数量不正确。";
        return std::nullopt;
    }
    for (int index = 0; index < defeatedCount; ++index) {
        std::string enemyId;
        if (!readText("defeated_enemy", enemyId)) {
            error = "存档中的敌人记录不完整。";
            return std::nullopt;
        }
        game.defeatedEnemies.insert(std::move(enemyId));
    }

    std::string trailing;
    if ((finishedValue != 0 && finishedValue != 1) || (input >> trailing)
        || !basicValuesAreValid(game)) {
        error = "存档包含多余内容或非法数值。";
        return std::nullopt;
    }

    error.clear();
    return game;
}

} // namespace dndmud
