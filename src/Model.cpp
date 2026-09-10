#include "dndmud/Model.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace dndmud {
namespace {

// 将英文字符转为小写。
std::string lowerEnglish(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

} // namespace

// 创建新玩家角色，根据职业设置初始属性。
Player Player::create(std::string name, PlayerClass playerClass) {
    Player player;
    player.name = name.empty() ? "冒险者" : std::move(name);
    player.playerClass = playerClass;

    switch (playerClass) {
    case PlayerClass::Warrior:
        player.maxHealth = 36;
        player.baseDefense = 2;
        player.baseAttack = 5;
        player.damageMinimum = 4;
        player.damageMaximum = 8;
        player.equippedWeapon = "rusty_sword";
        break;
    case PlayerClass::Ranger:
        player.maxHealth = 30;
        player.baseDefense = 3;
        player.baseAttack = 6;
        player.damageMinimum = 3;
        player.damageMaximum = 7;
        player.equippedWeapon = "hunter_bow";
        break;
    case PlayerClass::Mage:
        player.maxHealth = 26;
        player.baseDefense = 1;
        player.baseAttack = 6;
        player.damageMinimum = 2;
        player.damageMaximum = 6;
        player.equippedWeapon = "oak_staff";
        break;
    }

    player.health = player.maxHealth;
    player.equippedArmor = "leather_armor";
    player.inventory = {player.equippedWeapon, player.equippedArmor, "potion", "potion"};
    return player;
}

// 判断玩家是否存活。
bool Player::isAlive() const noexcept {
    return health > 0;
}

// 承受伤害，返回实际扣除的生命值。
int Player::takeDamage(int amount) noexcept {
    const int applied = std::min(health, std::max(0, amount));
    health -= applied;
    return applied;
}

// 恢复生命值，返回实际恢复量。
int Player::heal(int amount) noexcept {
    const int before = health;
    health = std::min(maxHealth, health + std::max(0, amount));
    return health - before;
}

// 向背包中添加物品。
void Player::addItem(const std::string& itemId) {
    inventory.push_back(itemId);
}

// 从背包中移除一个物品。
bool Player::removeItem(const std::string& itemId) {
    const auto found = std::find(inventory.begin(), inventory.end(), itemId);
    if (found == inventory.end()) {
        return false;
    }
    inventory.erase(found);
    return true;
}

// 返回背包中指定物品的数量。
int Player::itemCount(const std::string& itemId) const {
    return static_cast<int>(std::count(inventory.begin(), inventory.end(), itemId));
}

// 将文本解析为方向枚举，支持中英文。
std::optional<Direction> parseDirection(const std::string& text) {
    const std::string value = lowerEnglish(text);
    if (value == "north" || value == "n" || value == "北") return Direction::North;
    if (value == "east" || value == "e" || value == "东") return Direction::East;
    if (value == "south" || value == "s" || value == "南") return Direction::South;
    if (value == "west" || value == "w" || value == "西") return Direction::West;
    return std::nullopt;
}

// 返回方向的中英文名称。
std::string directionName(Direction direction) {
    switch (direction) {
    case Direction::North: return "north（北）";
    case Direction::East: return "east（东）";
    case Direction::South: return "south（南）";
    case Direction::West: return "west（西）";
    }
    return "未知";
}

// 返回职业的中文名称。
std::string className(PlayerClass playerClass) {
    switch (playerClass) {
    case PlayerClass::Warrior: return "战士";
    case PlayerClass::Ranger: return "游侠";
    case PlayerClass::Mage: return "法师";
    }
    return "未知职业";
}

} // namespace dndmud
