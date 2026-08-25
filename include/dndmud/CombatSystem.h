#pragma once

#include "dndmud/Character.h"
#include "dndmud/Random.h"

namespace dndmud {

/// 保存一次攻击过程和结果，供界面显示和测试检查。
struct AttackResult {
    int d20Roll{};          ///< 本次攻击随机生成的 1 到 20。
    int attackTotal{};      ///< 随机数加上角色攻击加成后的结果。
    int damage{};           ///< 本次计算伤害；未命中时为 0。
    bool hit{};             ///< 攻击是否命中。
    bool critical{};        ///< 是否得到最高随机数并造成双倍伤害。
    bool targetDefeated{};  ///< 攻击后目标是否已经死亡。
};

/// 计算攻击是否成功、造成多少伤害以及目标是否死亡。
class CombatSystem {
public:
    /// 使用外部提供的随机数工具；该工具在战斗系统使用期间必须存在。
    explicit CombatSystem(IRandomSource& randomSource);

    /// 执行一次攻击，扣除目标生命并返回完整结果。
    AttackResult attack(Character& attacker, Character& target);

private:
    IRandomSource& randomSource_;
};

} // namespace dndmud
