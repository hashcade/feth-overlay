// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/classes.hpp"
#include "feth/core/engine.hpp"

namespace feth::core {

bool class_is_available(CharacterId character, ClassId classId) {
  return classIsUnlocked(available_class_unlocks(character), classId);
}

void unlock_available_classes(ClassUnlocks& unlocks, CharacterId character) {
  const auto available = available_class_unlocks(character);
  unlocks.baseClassUnlocks |= available.baseClassUnlocks;
  unlocks.specialClassUnlocks |= available.specialClassUnlocks;
}

}  // namespace feth::core
