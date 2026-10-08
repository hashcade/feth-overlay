// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <utility>

namespace feth::core {

// convoy
using ItemId = uint16_t;
using ItemDurability = uint8_t;
using ItemAmount = uint8_t;
struct Item {
  ItemId id;
  ItemDurability durability;
  ItemAmount amount;
};
constexpr auto TOTAL_ITEM_COUNT = 400;
using ItemArray = std::array<Item, TOTAL_ITEM_COUNT>;
using ItemCount = int32_t;

static constexpr auto MAX_ITEM_ID = ItemId{0xFFFF};
static constexpr auto MAX_ITEM_DURABILITY = ItemDurability{100};
static constexpr auto MAX_ITEM_AMOUNT = ItemAmount{99};

// roster
using Battalion = uint64_t;
using RngValue = uint32_t;
using CharacterId = uint16_t;
using CharacterExp = uint16_t;
static constexpr auto CHARACTER_ITEM_COUNT = 6;
static constexpr auto CHARACTER_EQUIPPED_ITEM_COUNT = 2;
static constexpr auto CHARACTER_SKILL_COUNT = 11;

constexpr auto CLASS_UNLOCK_BIT_FIELD_DATA_SIZE = 8;
constexpr auto CLASS_UNLOCK_BIT_FIELD_SIZE =
  CLASS_UNLOCK_BIT_FIELD_DATA_SIZE * 8;
using ClassUnlockBitset = std::bitset<CLASS_UNLOCK_BIT_FIELD_SIZE>;
using ClassUnlockData = std::array<uint8_t, CLASS_UNLOCK_BIT_FIELD_DATA_SIZE>;

constexpr auto ROSTER_CHARACTER_COUNT = 60;

// support
using SupportPoint = uint16_t;
using SupportPair = std::pair<std::string, std::string>;

// classes
using ClassId = size_t;
static constexpr auto BASE_CLASS_ID_START = 0;
static constexpr auto SPECIAL_CLASS_ID_START = 84;

struct RosterEntry {
  int index;
  std::string name;
};

struct ClassUnlocks {
  ClassUnlockBitset baseClassUnlocks;
  ClassUnlockBitset specialClassUnlocks;
};

struct SupportEntry {
  size_t index;
  const SupportPair* p_pair;
};

struct SupportList {
  std::string displayName;
  std::list<std::shared_ptr<SupportEntry>> list;
};

struct SupportCollection {
  std::list<std::shared_ptr<SupportEntry>> allList;
  std::list<SupportList> supportListList;
};

}  // namespace feth::core
