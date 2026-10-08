// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/types.hpp"

#include <optional>
#include <span>

namespace feth::core {

std::optional<ItemDurability> normal_item_durability(ItemId id);
std::size_t refill_items(std::span<Item> items);

}  // namespace feth::core
