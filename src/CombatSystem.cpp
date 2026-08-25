#include "dndmud/CombatSystem.h"

#include <algorithm>

namespace dndmud {

CombatSystem::CombatSystem(IRandomSource& randomSource)
    : randomSource_(randomSource) {}

AttackResult CombatSystem::attack(Character& attacker, Character& target) {
    AttackResult result;
    result.d20Roll = randomSource_.uniform(1, 20);
    result.attackTotal = result.d20Roll + attacker.attackBonus();
    result.critical = result.d20Roll == 20;
    result.hit = result.d20Roll != 1
        && (result.critical || result.attackTotal >= target.armorClass());

    if (result.hit) {
        result.damage = std::max(
            1,
            randomSource_.uniform(1, attacker.damageSides()) + attacker.damageBonus());
        if (result.critical) {
            result.damage *= 2;
        }
        target.takeDamage(result.damage);
    }

    result.targetDefeated = !target.isAlive();
    return result;
}

} // namespace dndmud
