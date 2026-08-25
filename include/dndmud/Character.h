#pragma once

#include <string>

namespace dndmud {

struct CombatStats {
    int maxHealth{};
    int armorClass{};
    int attackBonus{};
    int damageBonus{};
    int damageSides{};
};

class Character {
public:
    Character(std::string name, CombatStats stats);
    virtual ~Character() = default;

    const std::string& name() const noexcept;
    int health() const noexcept;
    int maxHealth() const noexcept;
    int armorClass() const noexcept;
    int attackBonus() const noexcept;
    int damageBonus() const noexcept;
    int damageSides() const noexcept;
    bool isAlive() const noexcept;

    int takeDamage(int amount) noexcept;
    int heal(int amount) noexcept;
    void setHealth(int health) noexcept;

private:
    std::string name_;
    CombatStats stats_;
    int health_;
};

class Player final : public Character {
public:
    explicit Player(std::string name);

    int roomId() const noexcept;
    void setRoomId(int roomId) noexcept;
    int potionCount() const noexcept;
    void setPotionCount(int count) noexcept;
    bool consumePotion() noexcept;
    bool hasWon() const noexcept;
    void setWon(bool won) noexcept;

private:
    int roomId_{0};
    int potionCount_{1};
    bool won_{false};
};

class Enemy final : public Character {
public:
    Enemy(std::string id, std::string name, CombatStats stats);

    const std::string& id() const noexcept;

private:
    std::string id_;
};

} // namespace dndmud
