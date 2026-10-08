// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/types.hpp"

#include <set>

namespace feth::core {

struct ItemEditOptions {
  const std::set<ItemId>* ids{};
  const ItemDurability* durability{};
  const ItemAmount* amount{};
  bool addMissing{};
  bool normalDurability{};
};

void editItems(
  ItemArray& items, ItemCount& count, const ItemEditOptions& options
);
bool classIsUnlocked(const ClassUnlocks& unlocks, ClassId classId);
SupportCollection getSupportCollection();

}  // namespace feth::core
