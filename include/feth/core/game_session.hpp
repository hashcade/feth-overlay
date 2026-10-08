// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/abilities.hpp"
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
void setItemsWithIdSet(const ItemEditOptions& options);
void refillItemDurability();
PlayerResources getPlayerResources();
void setPlayerResources(const PlayerResources& resources);
BattalionArray getBattalions();
Battalion getBattalion(std::size_t index);
void setBattalion(std::size_t index, Battalion battalion);
void refillBattalionStamina();
LearnedAbilities getCharacterAbilities(std::size_t index);
void setCharacterAbilityLearned(std::size_t index, AbilityId id, bool learned);
std::list<core::RosterEntry> getRosterEntries();
core::ClassUnlocks getRosterCharacterClassUnlockAtIndex(std::size_t index);
void setClassUnlockAtIndexOfRosterCharacterAtIndex(
  std::size_t index, core::ClassId classId, bool unlocked
);
core::SupportPoint getSupportPointAtIndex(std::size_t index);
void setSupportPointAtIndex(std::size_t index, core::SupportPoint points);

}  // namespace feth::core
