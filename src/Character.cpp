#include "dndmud/Character.h"

#include <algorithm>
#include <utility>

namespace dndmud {

// 角色总是以满生命创建，避免产生“已死亡但未初始化”的对象。
Character::Character(std::string name, CombatStats stats)
    : name_(std::move(name)), stats_(stats), health_(stats.maxHealth) {}

const std::string& Character::name() const noexcept { return name_; }
int Character::health() const noexcept { return health_; }
int Character::maxHealth() const noexcept { return stats_.maxHealth; }
int Character::armorClass() const noexcept { return stats_.armorClass; }
int Character::attackBonus() const noexcept { return stats_.attackBonus; }
int Character::damageBonus() const noexcept { return stats_.damageBonus; }
int Character::damageSides() const noexcept { return stats_.damageSides; }
bool Character::isAlive() const noexcept { return health_ > 0; }

int Character::takeDamage(int amount) noexcept {
    // 同时限制负伤害和过量伤害，返回值反映真正扣除的生命。
    const int applied = std::min(health_, std::max(0, amount));
    health_ -= applied;
    return applied;
}

int Character::heal(int amount) noexcept {
    // 治疗不能反向造成伤害，也不能突破最大生命值。
    const int previous = health_;
    health_ = std::min(maxHealth(), health_ + std::max(0, amount));
    return health_ - previous;
}

void Character::setHealth(int health) noexcept {
    health_ = std::clamp(health, 0, maxHealth());
}

Player::Player(std::string name)
    // 当前演示版本中，玩家有 24 点生命；攻击较容易成功，伤害为 3 到 8。
    : Character(std::move(name), CombatStats{24, 12, 4, 2, 6}) {}

int Player::roomId() const noexcept { return roomId_; }
void Player::setRoomId(int roomId) noexcept { roomId_ = roomId; }
int Player::potionCount() const noexcept { return potionCount_; }
void Player::setPotionCount(int count) noexcept { potionCount_ = std::max(0, count); }

bool Player::consumePotion() noexcept {
    if (potionCount_ == 0) {
        return false;
    }
    --potionCount_;
    return true;
}

bool Player::hasWon() const noexcept { return won_; }
void Player::setWon(bool won) noexcept { won_ = won; }

Enemy::Enemy(std::string id, std::string name, CombatStats stats)
    : Character(std::move(name), stats), id_(std::move(id)) {}

const std::string& Enemy::id() const noexcept { return id_; }

} // namespace dndmud
