// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/locale.hpp"
#include "feth/core/types.hpp"

#include <set>

namespace feth::core {

bool initialize();
void shutdown();
bool gameIsRunning();
Locale detect_locale();
void setItemsWithIdSet(
  const std::set<core::ItemId>* ids,
  const core::ItemDurability* durability,
  const core::ItemAmount* amount,
  bool shouldAdd
);
std::list<core::RosterEntry> getRosterEntries();
core::ClassUnlocks getRosterCharacterClassUnlockAtIndex(std::size_t index);
void setClassUnlockAtIndexOfRosterCharacterAtIndex(
  std::size_t index, core::ClassId classId, bool unlocked
);
core::SupportPoint getSupportPointAtIndex(std::size_t index);
void setSupportPointAtIndex(std::size_t index, core::SupportPoint points);

}  // namespace feth::core
