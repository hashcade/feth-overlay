// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

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

  // I gave up mapping the whole thing here
  std::array<uint8_t, 0x8B> padding0;
  ClassUnlockData classUnlocks;
  std::array<uint8_t, 8> padding1;
  ClassUnlockData specialClassUnlocks;
  std::array<uint8_t, 0x161> padding2;
};
static_assert(sizeof(Character) == 0x24C);
static_assert(offsetof(Character, equippedBattalion) == 0x18);
static_assert(offsetof(Character, classUnlocks) == 0xD3);
using RosterCharacterArray = std::array<Character, ROSTER_CHARACTER_COUNT>;

// offsets
static constexpr auto ITEM_OFFSET = 0x01B121A0;
static constexpr auto ITEM_COUNT_OFFSET = ITEM_OFFSET + sizeof(ItemArray);
static constexpr auto ROSTER_OFFSET = ITEM_COUNT_OFFSET + sizeof(ItemCount);
static constexpr auto SUPPORT_OFFSET = ITEM_OFFSET + 0x24280;
// The v1.2.0 player block has 200 battalions followed by four uint32 fields
// before its support values (Player_V23 in feth-save-editor).
static constexpr auto BATTALION_OFFSET =
  SUPPORT_OFFSET - sizeof(BattalionArray) - 4 * sizeof(std::uint32_t);
static_assert(BATTALION_OFFSET == ITEM_OFFSET + 0x23C30);

}  // namespace feth::core
