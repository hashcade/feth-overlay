// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/durability_session.hpp"
#include "feth/core/game_profile.hpp"
#include "feth/core/items.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace feth::core {
namespace {

template <typename T>
T read(const ProcessMemory& memory, std::uint64_t address) {
  T value{};
  memory.read(address, std::as_writable_bytes(std::span(&value, 1)));
  return value;
}

struct Refill {
  std::uint64_t address;
  Item before;
  ItemDurability maximum;
};

void collect(
  std::vector<Refill>& edits, std::uint64_t address, std::span<const Item> items
) {
  for (std::size_t index = 0; index < items.size(); ++index) {
    const auto& item = items[index];
    const auto maximum = normal_item_durability(item.id);
    if (maximum && item.durability != *maximum)
      edits.push_back({address + index * sizeof(Item), item, *maximum});
  }
}

}  // namespace

void refill_item_durability(const ProcessMemory& memory, std::uint64_t main) {
  const auto save = read<std::uint64_t>(memory, main + SAVE_POINTER_OFFSET);
  if (save == 0)
    throw std::runtime_error("Save data unavailable");

  const auto count = read<ItemCount>(memory, save + sizeof(ItemArray));
  if (count < 0 || count > TOTAL_ITEM_COUNT)
    throw std::runtime_error("Invalid inventory size");
  const auto items = read<ItemArray>(memory, save);
  const auto roster_address = save + ROSTER_OFFSET - ITEM_OFFSET;
  const auto roster = read<RosterCharacterArray>(memory, roster_address);
  std::vector<Refill> edits;
  collect(edits, save, std::span(items).first(count));
  for (std::size_t index = 0; index < roster.size(); ++index) {
    if (roster[index].id <= 1200)
      collect(
        edits,
        roster_address + index * sizeof(Character),
        roster[index].heldItems
      );
  }

  const auto manager =
    read<std::uint64_t>(memory, main + BATTLE_MANAGER_POINTER_OFFSET);
  if (manager != 0) {
    const auto units = read<std::array<std::uint64_t, BATTLE_UNIT_COUNT>>(
      memory, manager + sizeof(std::uint64_t)
    );
    for (std::size_t index = 0; index < units.size(); ++index) {
      const auto address = units[index];
      if (address == 0)
        continue;
      const auto unit = read<std::array<std::byte, 0x101>>(memory, address);
      CharacterId id{};
      std::memcpy(&id, unit.data() + offsetof(Character, id), sizeof(id));
      const bool owned =
        std::ranges::any_of(roster, [id](const auto& character) {
          return character.id == id && id <= 1200;
        });
      if (
        !owned ||
        (std::to_integer<unsigned>(unit[BATTLE_UNIT_FLAGS_OFFSET]) & 1) == 0 ||
        std::to_integer<unsigned>(unit[BATTLE_UNIT_INDEX_OFFSET]) != index ||
        unit[BATTLE_UNIT_FACTION_OFFSET] != std::byte{0}
      )
        continue;
      std::array<Item, CHARACTER_ITEM_COUNT> held{};
      std::memcpy(held.data(), unit.data(), sizeof(held));
      collect(edits, address, held);
    }
  }

  if (
    read<std::uint64_t>(memory, main + SAVE_POINTER_OFFSET) != save ||
    read<std::uint64_t>(memory, main + BATTLE_MANAGER_POINTER_OFFSET) != manager
  )
    throw std::runtime_error("Inventory changed; try again");

  for (const auto& edit : edits) {
    const auto current = read<Item>(memory, edit.address);
    if (std::memcmp(&current, &edit.before, sizeof(Item)) != 0)
      throw std::runtime_error("Inventory changed; try again");
    memory.write(
      edit.address + offsetof(Item, durability),
      std::as_bytes(std::span(&edit.maximum, 1))
    );
    const auto after = read<Item>(memory, edit.address);
    if (
      after.id != current.id || after.amount != current.amount ||
      after.durability != edit.maximum
    )
      throw std::runtime_error("Durability verification failed");
  }
}

}  // namespace feth::core
