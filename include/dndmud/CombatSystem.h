#pragma once

#include "dndmud/Character.h"
#include "dndmud/Random.h"

namespace dndmud {

struct AttackResult {
    int d20Roll{};
    int attackTotal{};
    int damage{};
    bool hit{};
    bool critical{};
    bool targetDefeated{};
};

class CombatSystem {
public:
    explicit CombatSystem(IRandomSource& randomSource);

    AttackResult attack(Character& attacker, Character& target);

private:
    IRandomSource& randomSource_;
};

} // namespace dndmud
