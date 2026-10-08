// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/game_profile.hpp"
#include "feth/core/items.hpp"

#include <cassert>
#include <iostream>
#include <set>

using namespace feth::core;

int main() {
  // Normal maximums differ between weapons and consumables.
  assert(normal_item_durability(16) == 40);
  assert(normal_item_durability(1000) == 3);
  assert(!normal_item_durability(65535));
  std::array<Item, 3> worn{{{16, 1, 1}, {1000, 1, 2}, {65535, 7, 1}}};
  assert(refill_items(worn) == 2);
  assert(worn[0].durability == 40 && worn[0].amount == 1);
  assert(worn[1].durability == 3 && worn[1].amount == 2);
  assert(worn[2].durability == 7);
  assert(refill_items(worn) == 0);

  BattalionArray battalions{};
  battalions.fill({-1, 0, 0, EMPTY_BATTALION_TYPE, 80});
  battalions[0] = {3, 120, 1, 0, 4};
  battalions[1] = {-1, 5, 7, 99, 80};  // Unknown maximum: keep unchanged.
  assert(refill_battalions(battalions) == 1);
  assert((battalions[0] == Battalion{3, 120, 30, 0, 4}));
  assert(battalions[1].stamina == 7);
  assert(refill_battalions(battalions) == 0);
  const auto existing = battalions[0];
  assert(
    add_missing_battalions(battalions) == battalion_templates().size() - 1
  );
  assert(battalions[0] == existing && battalions[1].type == 99);
  assert(battalions[2].exp == 400 && battalions[2].characterId == -1);
  assert(add_missing_battalions(battalions) == 0);
  assert(!battalion_name(0, Locale::English).empty());
  std::array<Battalion, 1> full{{existing}};
  assert(add_missing_battalions(full) == 0 && full[0] == existing);
  std::array<Battalion, 1> free{{{-1, 0, 0, 200, 80}}};
  assert(add_missing_battalions(free) == 1);
  assert(free[0].type == 0 && free[0].stamina == 30 && free[0].skill == 4);

  ItemArray items{};
  items[0] = {1000, 10, 1};
  items[1] = {1001, 20, 2};
  ItemCount count = 2;
  const std::set<ItemId> selection{1000, 1002};

  // Owned-only editing updates the selected item without adding missing ones.
  editItems(items, count, {.ids = &selection, .amount = &MAX_ITEM_AMOUNT});
  assert(count == 2);
  assert(items[0].amount == 99 && items[0].durability == 10);
  assert(items[1].amount == 2);

  // Batch insertion uses the normal durability and quantity.
  editItems(items, count, {.ids = &selection, .addMissing = true});
  assert(count == 3);
  assert(
    items[2].id == 1002 &&
    items[2].durability == normal_item_durability(1002) && items[2].amount == 1
  );

  // The owned-items action applies to every current item.
  editItems(
    items,
    count,
    {.durability = &MAX_ITEM_DURABILITY, .amount = &MAX_ITEM_AMOUNT}
  );
  for (ItemCount index = 0; index < count; ++index) {
    assert(items[index].durability == 100 && items[index].amount == 99);
  }

  // A full convoy cannot grow or overwrite its existing entries.
  items.fill({1000, 10, 1});
  count = TOTAL_ITEM_COUNT;
  const std::set<ItemId> missing{1002};
  editItems(
    items,
    count,
    {.ids = &missing,
     .durability = &MAX_ITEM_DURABILITY,
     .amount = &MAX_ITEM_AMOUNT,
     .addMissing = true}
  );
  assert(count == TOTAL_ITEM_COUNT);
  assert(items.back().id == 1000 && items.back().amount == 1);

  // Insertion uses each item's normal durability; refilling never alters
  // amount.
  items.fill({});
  count = 0;
  const std::set<ItemId> sword{16};
  editItems(items, count, {.ids = &sword, .addMissing = true});
  assert(count == 1 && items[0].durability == 40);
  items[0].durability = 1;
  editItems(items, count, {.normalDurability = true});
  assert(items[0].durability == 40 && items[0].amount == 1);

  ClassUnlocks unlocks{};
  unlocks.baseClassUnlocks.set(0);
  unlocks.baseClassUnlocks.set(63);
  unlocks.specialClassUnlocks.set(0);
  assert(classIsUnlocked(unlocks, 0));
  assert(classIsUnlocked(unlocks, 63));
  assert(!classIsUnlocked(unlocks, 64));
  assert(classIsUnlocked(unlocks, 84));
  assert(!classIsUnlocked(unlocks, 85));

  // Both character menus must refer to the same indexed support pair.
  const auto supports = getSupportCollection();
  assert(supports.allList.size() == SUPPORT_LIST.size());
  std::size_t index{};
  for (const auto& entry : supports.allList) {
    assert(entry->index == index++);
    int occurrences{};
    for (const auto& character : supports.supportListList) {
      for (const auto& candidate : character.list) {
        if (candidate == entry) {
          assert(
            character.displayName == entry->p_pair->first ||
            character.displayName == entry->p_pair->second
          );
          ++occurrences;
        }
      }
    }
    assert(occurrences == 2);
  }

  std::cout << "Core regression checks passed\n";
}
