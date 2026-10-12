// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

#include <array>
#include <cstdint>

namespace feth::core {
using AbilityId = std::uint8_t;
using LearnedAbilities = std::array<std::uint8_t, 30>;
using EquippedAbilities = std::array<AbilityId, 5>;
constexpr AbilityId SWORD_AVOID_ABILITY_ID = 56;
constexpr AbilityId EMPTY_ABILITY_ID = 240;
constexpr std::size_t LEARNED_ABILITY_COUNT = 240;

bool ability_is_learned(const LearnedAbilities& abilities, AbilityId id);
void set_ability_learned(
  LearnedAbilities& abilities, AbilityId id, bool learned
);
}  // namespace feth::core
