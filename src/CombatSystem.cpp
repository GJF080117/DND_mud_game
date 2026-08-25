#include "dndmud/CombatSystem.h"

#include <algorithm>

namespace dndmud {

CombatSystem::CombatSystem(IRandomSource& randomSource)
    : randomSource_(randomSource) {}

AttackResult CombatSystem::attack(Character& attacker, Character& target) {
    AttackResult result;

    // 先生成 1 到 20：得到 1 时攻击失败，得到 20 时攻击成功并造成双倍伤害。
    result.d20Roll = randomSource_.uniform(1, 20);
    result.attackTotal = result.d20Roll + attacker.attackBonus();
    result.critical = result.d20Roll == 20;
    result.hit = result.d20Roll != 1
        && (result.critical || result.attackTotal >= target.armorClass());

    if (result.hit) {
        // 伤害最少为 1；得到最高攻击随机数时，将伤害翻倍。
        result.damage = std::max(
            1,
            randomSource_.uniform(1, attacker.damageSides()) + attacker.damageBonus());
        if (result.critical) {
            result.damage *= 2;
        }
        target.takeDamage(result.damage);
    }

    // 最后统一检查目标是否死亡，保证每次返回的信息都完整。
    result.targetDefeated = !target.isAlive();
    return result;
}

} // namespace dndmud
