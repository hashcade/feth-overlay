// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

#include "feth/core/locale.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <string>

namespace feth::core {
using AbilityId = std::uint8_t;
using LearnedAbilities = std::array<std::uint8_t, 30>;
constexpr std::size_t LEARNED_ABILITY_COUNT = 240;

std::span<const AbilityId> ability_ids();
std::string ability_name(AbilityId id, Locale locale);
bool ability_is_learned(const LearnedAbilities& abilities, AbilityId id);
void set_ability_learned(
  LearnedAbilities& abilities, AbilityId id, bool learned
);
}  // namespace feth::core
