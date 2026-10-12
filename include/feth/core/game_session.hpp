// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/abilities.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/locale.hpp"
#include "feth/core/resources.hpp"
#include "feth/core/types.hpp"

#include <set>

namespace feth::core {

bool initialize();
void shutdown();
bool gameIsRunning();
Locale detect_locale();
void fillItemCategory(const NamedItemIdSet& category, bool addMissing);
void fillOwnedItemAmounts();
void refillItemDurability();
PlayerResources getPlayerResources();
void setPlayerResources(const PlayerResources& resources);
void refillBattalionStamina();
void maxBattalionLevels();
void fillMissingBattalions();
std::list<core::RosterEntry> getRosterEntries();
core::ClassUnlocks getRosterCharacterClassUnlockAtIndex(std::size_t index);
void setClassUnlockAtIndexOfRosterCharacterAtIndex(
  std::size_t index, core::ClassId classId, bool unlocked
);
void unlockAvailableClasses(std::size_t index);
core::SupportPoint getSupportPointAtIndex(std::size_t index);
void setSupportPointAtIndex(std::size_t index, core::SupportPoint points);
void reachMaxSupportRank(std::size_t index);
void reachMaxSupportRanks(const SupportList& supports);

}  // namespace feth::core
