#pragma once

#include <string>

namespace dndmud {

/// 保存角色在战斗中使用的基本数值。
struct CombatStats {
    int maxHealth{};   ///< 生命值上限。
    int armorClass{};  ///< 对方需要达到的攻击数值。
    int attackBonus{}; ///< 攻击时额外增加的数值。
    int damageBonus{}; ///< 计算伤害时额外增加的数值。
    int damageSides{}; ///< 随机伤害的上限，例如 6 表示生成 1 到 6。
};

/// 玩家与敌人共有的角色类型，负责保存名称和生命值。
class Character {
public:
    /// 创建角色，并将当前生命值设置为最大生命值。
    Character(std::string name, CombatStats stats);
    virtual ~Character() = default;

    /// 以下函数只读取角色当前状态。
    const std::string& name() const noexcept;
    int health() const noexcept;
    int maxHealth() const noexcept;
    int armorClass() const noexcept;
    int attackBonus() const noexcept;
    int damageBonus() const noexcept;
    int damageSides() const noexcept;
    bool isAlive() const noexcept;

    /// 扣除生命并返回实际伤害；生命不会低于 0。
    int takeDamage(int amount) noexcept;
    /// 恢复生命并返回实际恢复量；生命不会超过上限。
    int heal(int amount) noexcept;
    /// 设置当前生命值；过小或过大的值会自动改到有效范围内。
    void setHealth(int health) noexcept;

private:
    std::string name_;
    CombatStats stats_;
    int health_;
};

/// 在普通角色信息之外，保存玩家的位置、药水和通关状态。
class Player final : public Character {
public:
    /// 使用当前演示版本预设的战斗数值创建玩家。
    explicit Player(std::string name);

    /// 查询或更新玩家所在房间 ID。
    int roomId() const noexcept;
    void setRoomId(int roomId) noexcept;
    /// 查询或更新药水数量；负数会自动改为 0。
    int potionCount() const noexcept;
    void setPotionCount(int count) noexcept;
    /// 尝试消耗一瓶药水；库存为空时返回 false。
    bool consumePotion() noexcept;
    /// 查询或更新玩家是否已经完成主线。
    bool hasWon() const noexcept;
    void setWon(bool won) noexcept;

private:
    int roomId_{0};
    int potionCount_{1};
    bool won_{false};
};

/// 保存唯一编号的敌人角色。
class Enemy final : public Character {
public:
    /// 创建敌人；房间通过 id 找到对应敌人。
    Enemy(std::string id, std::string name, CombatStats stats);

    /// 返回敌人的唯一编号。
    const std::string& id() const noexcept;

private:
    std::string id_;
};

} // namespace dndmud
