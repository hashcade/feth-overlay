// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/types.hpp"

#include <set>
#include <span>

namespace feth::core {

struct ItemFillOptions {
  const std::set<ItemId>& ids;
  bool addMissing{};
};

void fillConvoyItems(
  ItemArray& items, ItemCount& count, const ItemFillOptions& options
);
void fillItemCounts(
  std::span<ItemAmount> counts, const std::set<ItemId>& ids, bool addMissing
);
bool classIsUnlocked(const ClassUnlocks& unlocks, ClassId classId);
SupportCollection getSupportCollection();

}  // namespace feth::core
