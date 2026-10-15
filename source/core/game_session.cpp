// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#include "feth/core/game_session.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/classes.hpp"
#include "feth/core/durability_session.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/game_profile.hpp"
#include "feth/core/items.hpp"
#include "feth/core/supports.hpp"

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
        return locale_from_nacp_language(static_cast<std::int32_t>(index));
      }
      return Locale::English;
    }
    nsExit();
  }

  return Locale::English;
}

void refillItemDurability() {
  if (!gameIsRunning())
    throw std::runtime_error("Start Three Houses v1.2.0");
  const ProcessMemory memory{
    [](std::uint64_t address, std::span<std::byte> bytes) {
      TRY_THROW(
        dmntchtReadCheatProcessMemory(address, bytes.data(), bytes.size())
      );
    },
    [](std::uint64_t address, std::span<const std::byte> bytes) {
      TRY_THROW(
        dmntchtWriteCheatProcessMemory(address, bytes.data(), bytes.size())
      );
    },
  };
  refill_item_durability(memory, s_processMetadata.main_nso_extents.base);
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

void fillItemCategory(const NamedItemIdSet& category, bool addMissing) {
  if (!gameIsRunning())
    return;

  switch (category.storage) {
    case ItemStorage::Convoy: {
      auto items = read_game<ItemArray>(ITEM_OFFSET);
      auto count = read_game<ItemCount>(ITEM_COUNT_OFFSET);
      fillConvoyItems(items, count, {category.itemIdSet, addMissing});
      write_game(ITEM_OFFSET, items);
      write_game(ITEM_COUNT_OFFSET, count);
      break;
    }
    case ItemStorage::Misc: {
      auto counts = read_game<MiscItemCounts>(MISC_ITEM_OFFSET);
      fillItemCounts(counts, category.itemIdSet, addMissing);
      write_game(MISC_ITEM_OFFSET, counts);
      break;
    }
    case ItemStorage::Gifts: {
      auto counts = read_game<GiftItemCounts>(GIFT_ITEM_OFFSET);
      fillItemCounts(counts, category.itemIdSet, addMissing);
      write_game(GIFT_ITEM_OFFSET, counts);
      break;
    }
  }
}

void fillOwnedItemAmounts() {
  for (const auto& category : NAMED_ITEM_ID_SET_LIST)
    fillItemCategory(category, false);
}

PlayerResources getPlayerResources() {
  TRY_THROW(!gameIsRunning());
  return {
    read_game<std::uint32_t>(MONEY_OFFSET),
    read_game<std::uint32_t>(RENOWN_OFFSET)
  };
}

void setPlayerResources(const PlayerResources& resources) {
  if (!gameIsRunning())
    return;
  write_game(MONEY_OFFSET, resources.money);
  write_game(RENOWN_OFFSET, resources.renown);
}

static void updateBattalions(std::size_t (*action)(std::span<Battalion>)) {
  if (!gameIsRunning())
    return;
  auto battalions = read_game<BattalionArray>(BATTALION_OFFSET);
  const auto original = battalions;
  action(battalions);
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
      action(std::span(&battalion, 1))
    ) {
      write_game(
        ROSTER_OFFSET + index * sizeof(Character) +
          offsetof(Character, equippedBattalion),
        battalion
      );
    }
  }
}

void refillBattalionStamina() {
  updateBattalions(refill_battalions);
}

void maxBattalionLevels() {
  updateBattalions(max_battalion_levels);
}

void fillMissingBattalions() {
  if (!gameIsRunning())
    return;
  auto battalions = read_game<BattalionArray>(BATTALION_OFFSET);
  const auto original = battalions;
  fill_missing_battalions(battalions);
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
      result.push_back({curIdx, foundCharacterName->second, character.id});
    }
    curIdx++;
  }

  return result;
}

namespace {
ClassUnlocks class_unlocks(const Character& character) {
  ClassUnlocks result{};
  for (size_t bit = 0; bit < CLASS_UNLOCK_BIT_FIELD_SIZE; ++bit) {
    result.baseClassUnlocks[bit] =
      (character.classUnlocks[bit / 8] >> (bit % 8)) & 1;
    result.specialClassUnlocks[bit] =
      (character.specialClassUnlocks[bit / 8] >> (bit % 8)) & 1;
  }
  return result;
}

void write_dancer_ability(std::size_t offset, const Character& character) {
  write_game(
    offset + offsetof(Character, abilities) + SWORD_AVOID_ABILITY_ID / 8,
    character.abilities[SWORD_AVOID_ABILITY_ID / 8]
  );
  if (!ability_is_learned(character.abilities, SWORD_AVOID_ABILITY_ID))
    write_game(
      offset + offsetof(Character, equippedAbilities),
      character.equippedAbilities
    );
}
}  // namespace

core::ClassUnlocks
getRosterCharacterClassUnlockAtIndex(size_t rosterCharacterIndex) {
  TRY_THROW(!gameIsRunning());
  return class_unlocks(
    read_game<Character>(
      ROSTER_OFFSET + rosterCharacterIndex * sizeof(Character)
    )
  );
}

void setClassUnlockAtIndexOfRosterCharacterAtIndex(
  size_t rosterCharacterIndex, ClassId classId, bool isUnlocked
) {
  if (not gameIsRunning())
    return;

  const auto offset = ROSTER_OFFSET + rosterCharacterIndex * sizeof(Character);
  auto character = read_game<Character>(offset);
  if (!class_is_available(character.id, classId))
    return;

  const bool special = classId >= SPECIAL_CLASS_ID_START;
  const auto bit = special ? classId - SPECIAL_CLASS_ID_START : classId;
  auto& flags =
    special ? character.specialClassUnlocks : character.classUnlocks;
  const auto mask = uint8_t{1} << (bit % 8);
  auto& byte = flags[bit / 8];
  byte = isUnlocked ? byte | mask : byte & ~mask;
  write_game(
    offset +
      (special ? offsetof(Character, specialClassUnlocks)
               : offsetof(Character, classUnlocks)) +
      bit / 8,
    byte
  );

  if (classId == DANCER_CLASS_ID) {
    sync_dancer_ability(character, isUnlocked);
    write_dancer_ability(offset, character);
  }
}

void unlockAvailableClasses(std::size_t index) {
  TRY_THROW(!gameIsRunning());
  const auto offset = ROSTER_OFFSET + index * sizeof(Character);
  auto character = read_game<Character>(offset);
  auto unlocks = class_unlocks(character);
  unlock_available_classes(unlocks, character.id);
  const auto base = unlocks.baseClassUnlocks.to_ullong();
  const auto special = unlocks.specialClassUnlocks.to_ullong();
  for (std::size_t byte = 0; byte < character.classUnlocks.size(); ++byte) {
    character.classUnlocks[byte] = base >> (byte * 8);
    character.specialClassUnlocks[byte] = special >> (byte * 8);
  }
  write_game(
    offset + offsetof(Character, classUnlocks), character.classUnlocks
  );
  write_game(
    offset + offsetof(Character, specialClassUnlocks),
    character.specialClassUnlocks
  );

  if (class_is_available(character.id, DANCER_CLASS_ID)) {
    sync_dancer_ability(character, true);
    write_dancer_ability(offset, character);
  }
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

void reachMaxSupportRank(std::size_t index) {
  const auto target = max_support_points(index);
  if (getSupportPointAtIndex(index) < target)
    setSupportPointAtIndex(index, target);
}

void reachMaxSupportRanks(const SupportList& supports) {
  for (const auto& entry : supports.list)
    reachMaxSupportRank(entry->index);
}

}  // namespace feth::core
