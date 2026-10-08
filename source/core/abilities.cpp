// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#include "feth/core/abilities.hpp"

namespace feth::core {
bool ability_is_learned(const LearnedAbilities& abilities, AbilityId id) {
  return id < LEARNED_ABILITY_COUNT &&
         (abilities[id / 8] & (1 << (id % 8))) != 0;
}

void set_ability_learned(
  LearnedAbilities& abilities, AbilityId id, bool learned
) {
  if (id >= LEARNED_ABILITY_COUNT)
    return;
  auto& byte = abilities[id / 8];
  const auto mask = std::uint8_t{1} << (id % 8);
  byte = learned ? byte | mask : byte & ~mask;
}
}  // namespace feth::core
