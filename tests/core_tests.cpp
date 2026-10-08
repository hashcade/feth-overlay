// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"

#include <cassert>
#include <iostream>
#include <set>

using namespace feth::core;

int main() {
  ItemArray items{};
  items[0] = {1000, 10, 1};
  items[1] = {1001, 20, 2};
  ItemCount count = 2;
  const std::set<ItemId> selection{1000, 1002};

  // Owned-only editing updates the selected item without adding missing ones.
  editItems(items, count, &selection, nullptr, &MAX_ITEM_AMOUNT, false);
  assert(count == 2);
  assert(items[0].amount == 99 && items[0].durability == 10);
  assert(items[1].amount == 2);

  // Batch insertion keeps upstream's default durability and quantity.
  editItems(items, count, &selection, nullptr, nullptr, true);
  assert(count == 3);
  assert(
    items[2].id == 1002 && items[2].durability == 100 && items[2].amount == 1
  );

  // The owned-items action applies to every current item.
  editItems(
    items, count, nullptr, &MAX_ITEM_DURABILITY, &MAX_ITEM_AMOUNT, false
  );
  for (ItemCount index = 0; index < count; ++index) {
    assert(items[index].durability == 100 && items[index].amount == 99);
  }

  // A full convoy cannot grow or overwrite its existing entries.
  items.fill({1000, 10, 1});
  count = TOTAL_ITEM_COUNT;
  const std::set<ItemId> missing{1002};
  editItems(
    items, count, &missing, &MAX_ITEM_DURABILITY, &MAX_ITEM_AMOUNT, true
  );
  assert(count == TOTAL_ITEM_COUNT);
  assert(items.back().id == 1000 && items.back().amount == 1);

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
