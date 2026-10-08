// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#include "feth/core/game_session.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/game_profile.hpp"

#include <dmntcht.h>
#include <switch.h>

#include <algorithm>
#include <stdexcept>

#define TRY_THROW(expression)                                                  \
  if (Result rc = (expression); R_FAILED(rc)) {                                \
    throw std::runtime_error("Result code: " + std::to_string(rc));            \
  }

namespace feth::core {

bool initialize() {
  return R_SUCCEEDED(dmntchtInitialize());
}
void shutdown() {
  dmntchtExit();
}

static DmntCheatProcessMetadata s_processMetadata = {};

auto gameIsRunning() -> bool {
  auto hasCheatProcess = false;
  if (R_FAILED(dmntchtHasCheatProcess(&hasCheatProcess)))
    return false;
  if (not hasCheatProcess and R_FAILED(dmntchtForceOpenCheatProcess()))
    return false;

  // Refresh even when another overlay already opened the game process.
  if (R_FAILED(dmntchtGetCheatProcessMetadata(&s_processMetadata)))
    return false;

  return std::equal(
    TARGET_BID.begin(), TARGET_BID.end(), s_processMetadata.main_nso_build_id
  );
}

void setItemsWithIdSet(
  const std::set<ItemId>* p_itemIdSet,
  const ItemDurability* p_durabilityToSet,
  const ItemAmount* p_amountToSet,
  const bool shouldAdd
) {
  if (not gameIsRunning())
    return;

  // read items
  auto items = ItemArray{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ITEM_OFFSET, &items, sizeof(items)
  ));
  auto curItemCount = ItemCount{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ITEM_COUNT_OFFSET,
    &curItemCount,
    sizeof(curItemCount)
  ));

  core::editItems(
    items,
    curItemCount,
    p_itemIdSet,
    p_durabilityToSet,
    p_amountToSet,
    shouldAdd
  );

  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ITEM_OFFSET, &items, sizeof(items)
  ));
  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ITEM_COUNT_OFFSET,
    &curItemCount,
    sizeof(curItemCount)
  ));
}

std::list<core::RosterEntry> getRosterEntries() {
  TRY_THROW(!gameIsRunning());

  auto rosterCharacterArray = RosterCharacterArray{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ROSTER_OFFSET,
    &rosterCharacterArray,
    sizeof(rosterCharacterArray)
  ));

  auto result = std::list<RosterEntry>{};
  auto curIdx = 0;
  for (auto& character : rosterCharacterArray) {
    auto foundCharacterName = UNIT_ID_NAME_MAP.find(character.id);
    if (foundCharacterName != end(UNIT_ID_NAME_MAP)) {
      result.push_back({curIdx, foundCharacterName->second});
    }
    curIdx++;
  }

  return result;
}

core::ClassUnlocks
getRosterCharacterClassUnlockAtIndex(size_t rosterCharacterIndex) {
  TRY_THROW(!gameIsRunning());

  auto character = Character{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ROSTER_OFFSET +
      rosterCharacterIndex * sizeof(Character),
    &character,
    sizeof(character)
  ));

  ClassUnlocks result{};
  for (size_t bit = 0; bit < CLASS_UNLOCK_BIT_FIELD_SIZE; ++bit) {
    result.baseClassUnlocks[bit] =
      (character.classUnlocks[bit / 8] >> (bit % 8)) & 1;
    result.specialClassUnlocks[bit] =
      (character.specialClassUnlocks[bit / 8] >> (bit % 8)) & 1;
  }
  return result;
}

void setClassUnlockAtIndexOfRosterCharacterAtIndex(
  size_t rosterCharacterIndex, ClassId classId, bool isUnlocked
) {
  if (not gameIsRunning())
    return;

  auto character = Character{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ROSTER_OFFSET +
      rosterCharacterIndex * sizeof(Character),
    &character,
    sizeof(character)
  ));

  if (
    classId >= BASE_CLASS_ID_START and
    classId < BASE_CLASS_ID_START + CLASS_UNLOCK_BIT_FIELD_SIZE
  ) {
    const auto bit = classId - BASE_CLASS_ID_START;
    const auto mask = uint8_t{1} << (bit % 8);
    auto& byte = character.classUnlocks[bit / 8];
    byte = isUnlocked ? byte | mask : byte & ~mask;

  } else if (
    classId >= SPECIAL_CLASS_ID_START and
    classId < SPECIAL_CLASS_ID_START + CLASS_UNLOCK_BIT_FIELD_SIZE
  ) {
    const auto bit = classId - SPECIAL_CLASS_ID_START;
    const auto mask = uint8_t{1} << (bit % 8);
    auto& byte = character.specialClassUnlocks[bit / 8];
    byte = isUnlocked ? byte | mask : byte & ~mask;

  } else {
    // these probably aren't storable in characters
    return;
  }

  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ROSTER_OFFSET +
      rosterCharacterIndex * sizeof(Character),
    &character,
    sizeof(character)
  ));
}

core::SupportPoint getSupportPointAtIndex(size_t supportIndex) {
  TRY_THROW(!gameIsRunning());

  auto result = SupportPoint{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + SUPPORT_OFFSET +
      supportIndex * sizeof(result),
    &result,
    sizeof(result)
  ));

  return result;
}

void setSupportPointAtIndex(size_t supportIndex, SupportPoint points) {
  TRY_THROW(!gameIsRunning());
  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + SUPPORT_OFFSET +
      supportIndex * sizeof(points),
    &points,
    sizeof(points)
  ));
}

}  // namespace feth::core
