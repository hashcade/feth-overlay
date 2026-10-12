// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/types.hpp"

namespace feth::core {

ClassUnlocks available_class_unlocks(CharacterId character);
bool class_is_available(CharacterId character, ClassId classId);
void unlock_available_classes(ClassUnlocks& unlocks, CharacterId character);

}  // namespace feth::core
