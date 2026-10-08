// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#include "feth/core/battalions.hpp"

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

}  // namespace feth::core
