// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#include "feth/core/battalions.hpp"
#include <algorithm>

namespace feth::core {
const BattalionTemplate* battalion_template(std::uint8_t type) {
  for (const auto& entry : battalion_templates()) {
    if (entry.type == type)
      return &entry;
  }
  return nullptr;
}

int battalion_level(std::uint16_t experience) {
  return std::min(
    int(experience) / BATTALION_EXP_PER_LEVEL + 1, MAX_BATTALION_LEVEL
  );
}

void set_battalion_level(Battalion& battalion, int level) {
  level = std::clamp(level, 1, MAX_BATTALION_LEVEL);
  if (level != battalion_level(battalion.exp))
    battalion.exp = (level - 1) * BATTALION_EXP_PER_LEVEL;
}

void change_battalion_type(Battalion& battalion, std::uint8_t type) {
  if (battalion.type == type)
    return;
  const auto* entry = battalion_template(type);
  if (!entry)
    return;
  battalion.type = entry->type;
  battalion.skill = entry->skill;
  battalion.stamina = entry->stamina;
}

std::size_t refill_battalions(std::span<Battalion> battalions) {
  std::size_t changed{};
  for (auto& battalion : battalions) {
    const auto* entry = battalion_template(battalion.type);
    if (entry && battalion.stamina != entry->stamina) {
      battalion.stamina = entry->stamina;
      ++changed;
    }
  }
  return changed;
}

}  // namespace feth::core
