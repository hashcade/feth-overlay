// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/abilities.hpp"
#include "feth/core/types.hpp"
#include <cstddef>

namespace feth::core {

// target version
static const auto TARGET_BID =
  std::array<std::uint8_t, 0x20>{0x89, 0x4,  0x84, 0x49, 0xba, 0x23, 0x8c,
                                 0x8c, 0xf5, 0x65, 0x51, 0x8b, 0x83, 0xbf,
                                 0x2,  0xd3, 0x0,  0x0,  0x0,  0x0};

struct __attribute__((__packed__)) Character {
  std::array<Item, CHARACTER_ITEM_COUNT> heldItems;
  Battalion equippedBattalion;
  RngValue rngValue;
  CharacterId id;
  uint8_t unk26[6];
  CharacterExp exp;
  std::array<ItemId, CHARACTER_EQUIPPED_ITEM_COUNT> equippedItems;
  std::array<CharacterExp, CHARACTER_SKILL_COUNT> skillExps;

  std::array<uint8_t, 0x19> padding0;
  LearnedAbilities abilities;
  EquippedAbilities equippedAbilities;
  std::array<uint8_t, 0x4F> paddingAbilities;
  ClassUnlockData classUnlocks;
  std::array<uint8_t, 8> padding1;
  ClassUnlockData specialClassUnlocks;
  std::array<uint8_t, 0x161> padding2;
};
static_assert(sizeof(Character) == 0x24C);
static_assert(offsetof(Character, equippedBattalion) == 0x18);
static_assert(offsetof(Character, abilities) == 0x61);
static_assert(offsetof(Character, equippedAbilities) == 0x7F);
static_assert(offsetof(Character, classUnlocks) == 0xD3);
static_assert(offsetof(Character, specialClassUnlocks) == 0xE3);
using RosterCharacterArray = std::array<Character, ROSTER_CHARACTER_COUNT>;

// offsets
static constexpr auto ITEM_OFFSET = 0x01B121A0;
static constexpr auto ITEM_COUNT_OFFSET = ITEM_OFFSET + sizeof(ItemArray);
static constexpr auto ROSTER_OFFSET = ITEM_COUNT_OFFSET + sizeof(ItemCount);
static constexpr auto SAVE_POINTER_OFFSET = ITEM_OFFSET - 0x10;
// v1.2.0: main+0x1AC3730 points to a manager with 105 unit pointers at +8.
static constexpr auto BATTLE_MANAGER_POINTER_OFFSET = 0x01AC3730;
static constexpr auto BATTLE_UNIT_COUNT = 105;
static constexpr auto BATTLE_UNIT_FLAGS_OFFSET = 0xD6;
static constexpr auto BATTLE_UNIT_INDEX_OFFSET = 0xF0;
static constexpr auto BATTLE_UNIT_FACTION_OFFSET = 0x100;
static constexpr auto SUPPORT_OFFSET = ITEM_OFFSET + 0x24280;
// Player_V23.Money and Activities_V23.Reputation, verified against the editor
// layouts and Gamerjin's v1.2.0 resource codes (see docs/development.md).
static constexpr auto MONEY_OFFSET = ITEM_OFFSET + 0x24274;
static constexpr auto RENOWN_OFFSET = ITEM_OFFSET + 0x250D4;
// The v1.2.0 player block has 200 battalions followed by four uint32 fields
// before its support values (Player_V23 in feth-save-editor).
static constexpr auto BATTALION_OFFSET =
  SUPPORT_OFFSET - sizeof(BattalionArray) - 4 * sizeof(std::uint32_t);
static_assert(BATTALION_OFFSET == ITEM_OFFSET + 0x23C30);

// Player_V23 offsets, anchored to CharacterSupportValues at 0x1080.
static constexpr auto PLAYER_OFFSET = SUPPORT_OFFSET - 0x1080;
static constexpr auto MISC_ITEM_OFFSET = PLAYER_OFFSET + 0x13B1;
static constexpr auto GIFT_ITEM_OFFSET = PLAYER_OFFSET + 0x1490;
using MiscItemCounts = std::array<ItemAmount, 223>;
using GiftItemCounts = std::array<ItemAmount, 225>;
static_assert(MISC_ITEM_OFFSET + sizeof(MiscItemCounts) == GIFT_ITEM_OFFSET);

}  // namespace feth::core
