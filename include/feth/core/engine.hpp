// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/types.hpp"

#include <set>

namespace feth::core {

void editItems(
  ItemArray& items,
  ItemCount& count,
  const std::set<ItemId>* ids,
  const ItemDurability* durability,
  const ItemAmount* amount,
  bool shouldAdd
);
bool classIsUnlocked(const ClassUnlocks& unlocks, ClassId classId);
SupportCollection getSupportCollection();

}  // namespace feth::core
