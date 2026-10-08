// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#include "feth/core/game_session.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/game_profile.hpp"
#include "feth/core/items.hpp"

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

Locale detect_locale() {
  // Follow MHGU: prefer the console language, then the game's metadata.
  u64 language_code{};
  SetLanguage language{};
  if (R_SUCCEEDED(setInitialize())) {
    const auto result = setGetSystemLanguage(&language_code);
    const auto mapping =
      R_SUCCEEDED(result) ? setMakeLanguage(language_code, &language) : result;
    setExit();
    if (R_SUCCEEDED(mapping)) {
      return locale_from_switch_language(static_cast<std::int32_t>(language));
    }
  }

  if (gameIsRunning() && R_SUCCEEDED(nsInitialize())) {
    auto control = std::make_unique<NsApplicationControlData>();
    u64 actual_size{};
    NacpLanguageEntry* desired{};
    const auto result = nsGetApplicationControlData(
      NsApplicationControlSource_Storage,
      s_processMetadata.title_id,
      control.get(),
      sizeof(*control),
      &actual_size
    );
    if (
      R_SUCCEEDED(result) && actual_size >= sizeof(NacpStruct) &&
      R_SUCCEEDED(nsGetApplicationDesiredLanguage(&control->nacp, &desired)) &&
      desired != nullptr
    ) {
      const auto index =
        desired - reinterpret_cast<const NacpLanguageEntry*>(&control->nacp);
      nsExit();
      if (index >= 0 && index < 16) {
        return locale_from_switch_language(static_cast<std::int32_t>(index));
      }
      return Locale::English;
    }
    nsExit();
  }

  return Locale::English;
}

void setItemsWithIdSet(const ItemEditOptions& options) {
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

  core::editItems(items, curItemCount, options);

  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ITEM_OFFSET, &items, sizeof(items)
  ));
  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + ITEM_COUNT_OFFSET,
    &curItemCount,
    sizeof(curItemCount)
  ));
}

void refillItemDurability() {
  if (!gameIsRunning())
    return;
  ItemArray items{};
  ItemCount count{};
  const auto base = s_processMetadata.main_nso_extents.base;
  TRY_THROW(
    dmntchtReadCheatProcessMemory(base + ITEM_OFFSET, &items, sizeof(items))
  );
  TRY_THROW(dmntchtReadCheatProcessMemory(
    base + ITEM_COUNT_OFFSET, &count, sizeof(count)
  ));
  if (refill_items(std::span(items).first(count))) {
    TRY_THROW(
      dmntchtWriteCheatProcessMemory(base + ITEM_OFFSET, &items, sizeof(items))
    );
  }

  RosterCharacterArray roster{};
  TRY_THROW(
    dmntchtReadCheatProcessMemory(base + ROSTER_OFFSET, &roster, sizeof(roster))
  );
  for (std::size_t index = 0; index < roster.size(); ++index) {
    auto held_items = roster[index].heldItems;
    if (refill_items(held_items)) {
      TRY_THROW(dmntchtWriteCheatProcessMemory(
        base + ROSTER_OFFSET + index * sizeof(Character),
        &held_items,
        sizeof(held_items)
      ));
    }
  }
}

namespace {
template <typename T> T read_game(std::size_t offset) {
  T value{};
  TRY_THROW(dmntchtReadCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + offset, &value, sizeof(value)
  ));
  return value;
}

template <typename T> void write_game(std::size_t offset, const T& value) {
  TRY_THROW(dmntchtWriteCheatProcessMemory(
    s_processMetadata.main_nso_extents.base + offset, &value, sizeof(value)
  ));
}
}  // namespace

BattalionArray getBattalions() {
  TRY_THROW(!gameIsRunning());
  return read_game<BattalionArray>(BATTALION_OFFSET);
}

Battalion getBattalion(std::size_t index) {
  TRY_THROW(!gameIsRunning());
  auto battalion =
    read_game<Battalion>(BATTALION_OFFSET + index * sizeof(Battalion));
  if (battalion.characterId >= 0) {
    const auto roster = read_game<RosterCharacterArray>(ROSTER_OFFSET);
    for (const auto& character : roster) {
      if (
        character.id == battalion.characterId &&
        character.equippedBattalion.type == battalion.type
      ) {
        // Active battles can update the equipped copy first.
        battalion.exp = character.equippedBattalion.exp;
        battalion.stamina = character.equippedBattalion.stamina;
        break;
      }
    }
  }
  return battalion;
}

void setBattalion(std::size_t index, Battalion battalion) {
  if (!gameIsRunning())
    return;
  const auto offset = BATTALION_OFFSET + index * sizeof(Battalion);
  const auto original = read_game<Battalion>(offset);
  if (original.characterId >= 0) {
    const auto roster = read_game<RosterCharacterArray>(ROSTER_OFFSET);
    for (std::size_t slot = 0; slot < roster.size(); ++slot) {
      if (
        roster[slot].id == original.characterId &&
        roster[slot].equippedBattalion.type == original.type
      ) {
        write_game(
          ROSTER_OFFSET + slot * sizeof(Character) +
            offsetof(Character, equippedBattalion),
          battalion
        );
        break;
      }
    }
  }
  write_game(offset, battalion);
}

void refillBattalionStamina() {
  if (!gameIsRunning())
    return;
  auto battalions = read_game<BattalionArray>(BATTALION_OFFSET);
  const auto original = battalions;
  refill_battalions(battalions);
  for (std::size_t index = 0; index < battalions.size(); ++index) {
    if (battalions[index] != original[index])
      write_game(
        BATTALION_OFFSET + index * sizeof(Battalion), battalions[index]
      );
  }

  const auto roster = read_game<RosterCharacterArray>(ROSTER_OFFSET);
  for (std::size_t index = 0; index < roster.size(); ++index) {
    auto battalion = roster[index].equippedBattalion;
    if (
      UNIT_ID_NAME_MAP.contains(roster[index].id) &&
      refill_battalions(std::span(&battalion, 1))
    ) {
      write_game(
        ROSTER_OFFSET + index * sizeof(Character) +
          offsetof(Character, equippedBattalion),
        battalion
      );
    }
  }
}

void addMissingBattalions() {
  if (!gameIsRunning())
    return;
  auto battalions = read_game<BattalionArray>(BATTALION_OFFSET);
  const auto original = battalions;
  add_missing_battalions(battalions);
  for (std::size_t index = 0; index < battalions.size(); ++index) {
    if (battalions[index] != original[index])
      write_game(
        BATTALION_OFFSET + index * sizeof(Battalion), battalions[index]
      );
  }
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
