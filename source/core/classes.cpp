// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/classes.hpp"
#include "feth/core/engine.hpp"

#include <algorithm>

namespace feth::core {

void sync_dancer_ability(Character& character, bool unlocked) {
  set_ability_learned(character.abilities, SWORD_AVOID_ABILITY_ID, unlocked);
  if (!unlocked)
    std::replace(
      character.equippedAbilities.begin(),
      character.equippedAbilities.end(),
      SWORD_AVOID_ABILITY_ID,
      EMPTY_ABILITY_ID
    );
}

bool class_is_available(CharacterId character, ClassId classId) {
  return classIsUnlocked(available_class_unlocks(character), classId);
}

void unlock_available_classes(ClassUnlocks& unlocks, CharacterId character) {
  const auto available = available_class_unlocks(character);
  unlocks.baseClassUnlocks |= available.baseClassUnlocks;
  unlocks.specialClassUnlocks |= available.specialClassUnlocks;
}

}  // namespace feth::core
