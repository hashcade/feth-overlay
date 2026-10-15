// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/durability_session.hpp"
#include "feth/core/game_profile.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

using namespace feth::core;

namespace {

constexpr std::uint64_t kSave = 0x10000000;
constexpr std::uint64_t kManager = 0x20000000;
constexpr std::uint64_t kUnit = 0x30000000;

struct Fixture {
  std::map<std::uint64_t, std::vector<std::byte>> blocks;
  std::vector<std::uint64_t> writes;
  bool discard_writes{};

  Fixture() {
    blocks[SAVE_POINTER_OFFSET].resize(8);
    blocks[BATTLE_MANAGER_POINTER_OFFSET].resize(8);
    blocks[kSave].resize(sizeof(ItemArray) + 4 + sizeof(RosterCharacterArray));
    put(SAVE_POINTER_OFFSET, kSave);
    RosterCharacterArray roster{};
    for (auto& character : roster)
      character.id = 0xFFFF;
    roster[0].id = 0;
    roster[0].heldItems[0] = {16, 2, 1};
    put(kSave + ROSTER_OFFSET - ITEM_OFFSET, roster);
    ItemArray items{};
    items[0] = {16, 1, 1};
    items[1] = {1000, 1, 2};
    items[2] = {65535, 7, 1};
    put(kSave, items);
    put(kSave + sizeof(ItemArray), ItemCount{3});
  }

  std::span<std::byte> at(std::uint64_t address, std::size_t size) {
    auto it = blocks.upper_bound(address);
    if (it == blocks.begin())
      throw std::runtime_error("Unmapped address");
    --it;
    const auto offset = address - it->first;
    if (offset > it->second.size() || size > it->second.size() - offset)
      throw std::runtime_error("Out-of-bounds access");
    return std::span(it->second).subspan(offset, size);
  }

  template <typename T> void put(std::uint64_t address, const T& value) {
    std::memcpy(at(address, sizeof(T)).data(), &value, sizeof(T));
  }

  ProcessMemory memory() {
    return {
      [this](auto address, auto bytes) {
        const auto source = at(address, bytes.size());
        std::memcpy(bytes.data(), source.data(), bytes.size());
      },
      [this](auto address, auto bytes) {
        assert(bytes.size() == 1);
        writes.push_back(address);
        if (!discard_writes)
          std::memcpy(
            at(address, bytes.size()).data(), bytes.data(), bytes.size()
          );
      },
    };
  }

  void battle() {
    blocks[kManager].resize(0x358);
    put(BATTLE_MANAGER_POINTER_OFFSET, kManager);
    std::array<std::uint64_t, BATTLE_UNIT_COUNT> units{};
    for (unsigned index = 0; index < 5; ++index) {
      const auto address = kUnit + index * 0x1000;
      units[index] = address;
      blocks[address].resize(0x110);
      put(address, Item{16, 3, 1});
      put(address + offsetof(Character, id), CharacterId{0});
      put(address + BATTLE_UNIT_FLAGS_OFFSET, std::uint8_t{1});
      put(address + BATTLE_UNIT_INDEX_OFFSET, static_cast<std::uint8_t>(index));
    }
    put(
      kUnit + 0x1000 + BATTLE_UNIT_FACTION_OFFSET, std::uint8_t{1}
    );  // Enemy with the same ID.
    put(
      kUnit + 0x2000 + BATTLE_UNIT_FLAGS_OFFSET, std::uint8_t{0}
    );  // Inactive.
    put(
      kUnit + 0x3000 + offsetof(Character, id), CharacterId{25}
    );  // Not in the roster.
    put(
      kUnit + 0x4000 + BATTLE_UNIT_FACTION_OFFSET, std::uint8_t{2}
    );  // Green ally.
    put(kManager + 8, units);
  }
};

void expect_failure(Fixture& fixture, const char* message) {
  bool failed{};
  try {
    refill_item_durability(fixture.memory(), 0);
  } catch (const std::runtime_error& error) {
    failed = true;
    assert(std::string(error.what()) == message);
  }
  assert(failed);
}

}  // namespace

int main() {
  for (bool in_battle : {false, true}) {
    Fixture fixture;
    if (in_battle)
      fixture.battle();
    const auto before = fixture.blocks;
    refill_item_durability(fixture.memory(), 0);
    assert(fixture.writes.size() == (in_battle ? 4 : 3));
    assert(fixture.at(kSave + 2, 1)[0] == std::byte{40});
    assert(fixture.at(kSave + 6, 1)[0] == std::byte{3});
    if (in_battle) {
      assert(fixture.at(kUnit + 2, 1)[0] == std::byte{40});
      for (unsigned index = 1; index < 5; ++index)
        assert(
          fixture.blocks[kUnit + index * 0x1000] ==
          before.at(kUnit + index * 0x1000)
        );
    }
    // No IDs, amounts, flags, HP, positions, or other state may be written.
    for (const auto& [address, bytes] : before) {
      for (std::size_t index = 0; index < bytes.size(); ++index) {
        if (bytes[index] != fixture.blocks[address][index]) {
          bool durability_byte{};
          for (const auto written : fixture.writes)
            durability_byte |= written == address + index;
          assert(durability_byte);
        }
      }
    }
    fixture.writes.clear();
    refill_item_durability(fixture.memory(), 0);
    assert(fixture.writes.empty());
  }

  for (const ItemCount count : {-1, 401}) {
    Fixture fixture;
    fixture.put(kSave + sizeof(ItemArray), count);
    expect_failure(fixture, "Invalid inventory size");
    assert(fixture.writes.empty());
  }
  {
    Fixture fixture;
    fixture.put(SAVE_POINTER_OFFSET, std::uint64_t{});
    expect_failure(fixture, "Save data unavailable");
    assert(fixture.writes.empty());
  }
  {
    Fixture fixture;
    fixture.discard_writes = true;
    expect_failure(fixture, "Durability verification failed");
    assert(fixture.writes.size() == 1);
  }
  {
    Fixture fixture;
    auto memory = fixture.memory();
    const auto read = memory.read;
    int save_reads{};
    memory.read = [&](auto address, auto bytes) {
      if (address == SAVE_POINTER_OFFSET && ++save_reads == 2)
        fixture.put(SAVE_POINTER_OFFSET, std::uint64_t{kSave + 4});
      read(address, bytes);
    };
    bool failed{};
    try {
      refill_item_durability(memory, 0);
    } catch (const std::runtime_error& error) {
      failed = true;
      assert(std::string(error.what()) == "Inventory changed; try again");
    }
    assert(failed && fixture.writes.empty());
  }
  std::cout << "Durability session checks passed\n";
}
