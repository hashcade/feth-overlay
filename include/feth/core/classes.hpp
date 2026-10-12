// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/game_profile.hpp"

namespace feth::core {

constexpr ClassId DANCER_CLASS_ID = 43;
void sync_dancer_ability(Character& character, bool unlocked);

ClassUnlocks available_class_unlocks(CharacterId character);
bool class_is_available(CharacterId character, ClassId classId);
void unlock_available_classes(ClassUnlocks& unlocks, CharacterId character);

}  // namespace feth::core
