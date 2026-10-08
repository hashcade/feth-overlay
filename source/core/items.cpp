// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/items.hpp"

namespace feth::core {

std::size_t refill_items(std::span<Item> items) {
  std::size_t changed{};
  for (auto& item : items) {
    const auto maximum = normal_item_durability(item.id);
    if (maximum && item.durability != *maximum) {
      item.durability = *maximum;
      ++changed;
    }
  }
  return changed;
}

}  // namespace feth::core
