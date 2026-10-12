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

std::size_t max_battalion_levels(std::span<Battalion> battalions) {
  std::size_t changed{};
  constexpr auto experience =
    (MAX_BATTALION_LEVEL - 1) * BATTALION_EXP_PER_LEVEL;
  for (auto& battalion : battalions) {
    if (battalion.type < EMPTY_BATTALION_TYPE && battalion.exp < experience) {
      battalion.exp = experience;
      ++changed;
    }
  }
  return changed;
}

std::size_t fill_missing_battalions(std::span<Battalion> battalions) {
  std::size_t added{};
  for (const auto& entry : battalion_templates()) {
    if (std::ranges::any_of(battalions, [&entry](const auto& battalion) {
          return battalion.type == entry.type;
        }))
      continue;
    const auto empty =
      std::ranges::find_if(battalions, [](const auto& battalion) {
        return battalion.type >= EMPTY_BATTALION_TYPE;
      });
    if (empty == battalions.end())
      break;
    *empty = {-1, 0, entry.stamina, entry.type, entry.skill};
    ++added;
  }
  return added;
}

}  // namespace feth::core
