// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/game_profile.hpp"
#include "feth/core/items.hpp"

#include <bit>
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

  LearnedAbilities abilities{};
  for (const auto id : {0, 7, 8, 239})
    set_ability_learned(abilities, id, true);
  assert(abilities[0] == 0x81 && abilities[1] == 1);
  assert(abilities.back() == 0x80);
  assert(!ability_is_learned(abilities, 240));
  set_ability_learned(abilities, 8, false);
  assert(abilities[1] == 0);
  assert(ability_is_learned(abilities, 0) && ability_is_learned(abilities, 7));
  const auto unchanged = abilities;
  set_ability_learned(abilities, 250, true);
  assert(abilities == unchanged);
  for (const auto id : ability_ids()) {
    assert(id < LEARNED_ABILITY_COUNT);
    for (const auto locale :
         {Locale::English, Locale::SimplifiedChinese, Locale::Japanese})
      assert(!ability_name(id, locale).empty());
  }

  // Sword Avo +20 is an ability flag, not a weapon-skill rank or experience.
  assert(ability_name(56, Locale::SimplifiedChinese) == "剑回避+20");
  Character character{};
  character.skillExps.fill(1234);
  using CharacterBytes = std::array<std::uint8_t, sizeof(Character)>;
  const auto before_ability = std::bit_cast<CharacterBytes>(character);
  set_ability_learned(character.abilities, 56, true);
  assert(ability_is_learned(character.abilities, 56));
  const auto after_ability = std::bit_cast<CharacterBytes>(character);
  for (std::size_t index = 0; index < after_ability.size(); ++index) {
    if (index == 0x68)
      assert(after_ability[index] == 1);
    else
      assert(after_ability[index] == before_ability[index]);
  }
  set_ability_learned(character.abilities, 56, false);
  assert(std::bit_cast<CharacterBytes>(character) == before_ability);

  BattalionArray battalions{};
  battalions.fill({-1, 0, 0, EMPTY_BATTALION_TYPE, 80});
  battalions[0] = {3, 120, 1, 0, 4};
  battalions[1] = {-1, 5, 7, 99, 80};  // Unknown maximum: keep unchanged.
  assert(refill_battalions(battalions) == 1);
  assert((battalions[0] == Battalion{3, 120, 30, 0, 4}));
  assert(battalions[1].stamina == 7);
  assert(refill_battalions(battalions) == 0);
  assert(battalion_level(0) == 1 && battalion_level(99) == 1);
  assert(battalion_level(100) == 2 && battalion_level(399) == 4);
  assert(battalion_level(400) == 5);
  Battalion leveled{3, 120, 30, 0, 4};
  set_battalion_level(leveled, 2);
  assert(leveled.exp == 120);  // An unchanged level preserves progress.
  for (int level = 1; level <= 5; ++level) {
    set_battalion_level(leveled, level);
    assert(battalion_level(leveled.exp) == level);
    assert(leveled.stamina == 30 && leveled.type == 0 && leveled.skill == 4);
  }
  auto changed_type = battalions[0];
  change_battalion_type(changed_type, 70);
  assert((changed_type == Battalion{3, 120, 120, 70, 29}));
  assert(battalion_level(changed_type.exp) == 2);
  changed_type.skill =
    0;  // A custom gambit remains when the type is unchanged.
  changed_type.stamina = 1;
  change_battalion_type(changed_type, 70);
  assert(changed_type.skill == 0 && changed_type.stamina == 1);
  change_battalion_type(changed_type, 0);
  assert(changed_type.skill == 4 && changed_type.stamina == 30);
  // Category fills top up existing stacks and add each missing type once.
  ItemArray items{};
  items[0] = {1000, 1, 99};
  items[1] = {1001, 2, 7};
  items[2] = {16, 5, 1};  // Weapons never participate in quantity fills.
  items[3] = {1002, 0, 0};
  items[4] = {1161, 0, 0};
  ItemCount count = 5;
  const std::set<ItemId> potions{1000, 1001, 1002, 1011, 1012};

  fillConvoyItems(items, count, {potions, false});
  assert(count == 5);
  assert(items[0].amount == 99 && items[0].durability == 1);
  assert(items[1].amount == 99 && items[1].durability == 2);
  assert(items[2].amount == 1 && items[2].durability == 5);
  assert(items[3].amount == 0 && items[4].amount == 0);

  fillConvoyItems(items, count, {potions, true});
  assert(count == 7);
  assert(items[3].amount == 99 && items[3].durability == 0);
  assert(items[4].amount == 0);
  assert(items[5].id == 1011 && items[5].amount == 99);
  assert(items[5].durability == normal_item_durability(1011));
  assert(items[6].id == 1012 && items[6].amount == 99);
  const auto topped_up =
    std::bit_cast<std::array<std::uint8_t, sizeof(items)>>(items);
  fillConvoyItems(items, count, {potions, true});
  assert(count == 7);
  assert(
    (std::bit_cast<std::array<std::uint8_t, sizeof(items)>>(items) == topped_up)
  );

  // A full convoy cannot grow or overwrite unrelated slots.
  items.fill({16, 10, 1});
  count = TOTAL_ITEM_COUNT;
  fillConvoyItems(items, count, {potions, true});
  assert(count == TOTAL_ITEM_COUNT);
  for (const auto& item : items)
    assert(item.id == 16 && item.amount == 1 && item.durability == 10);

  // The global fill uses exactly the category lists, across each storage.
  MiscItemCounts misc{};
  GiftItemCounts gifts{};
  misc[64] = 3;    // Smithing Stone.
  misc[68] = 99;   // Mythril.
  misc[23] = 1;    // Herring Bait.
  misc[30] = 7;    // Quest-only Tournament Bait.
  misc[72] = 4;    // Unused ore slot.
  gifts[20] = 1;   // Owl Feather.
  gifts[197] = 2;  // Pitcher Plant.
  gifts[38] = 1;   // Lost White Glove.
  gifts[196] = 1;  // Anna's separate quest object, not a gift.
  items.fill({});
  items[0] = {16, 5, 1};
  items[1] = {1002, 2, 5};
  items[2] = {1161, 0, 0};
  count = 3;
  for (const auto& category : NAMED_ITEM_ID_SET_LIST) {
    switch (category.storage) {
      case ItemStorage::Convoy:
        fillConvoyItems(items, count, {category.itemIdSet, false});
        break;
      case ItemStorage::Misc:
        fillItemCounts(misc, category.itemIdSet, false);
        break;
      case ItemStorage::Gifts:
        fillItemCounts(gifts, category.itemIdSet, false);
        break;
    }
  }
  assert(items[0].amount == 1 && items[1].amount == 99);
  assert(items[2].amount == 0 && count == 3);
  assert(misc[64] == 99 && misc[68] == 99 && misc[23] == 99);
  assert(misc[65] == 0 && misc[30] == 7 && misc[72] == 4);
  assert(gifts[20] == 99 && gifts[197] == 99 && gifts[0] == 0);
  assert(gifts[38] == 1 && gifts[196] == 1);

  for (const auto& category : NAMED_ITEM_ID_SET_LIST) {
    if (category.storage == ItemStorage::Misc)
      fillItemCounts(misc, category.itemIdSet, true);
    else if (category.storage == ItemStorage::Gifts)
      fillItemCounts(gifts, category.itemIdSet, true);
  }
  assert(misc[65] == 99 && misc[30] == 7 && misc[72] == 4);
  assert(gifts[0] == 99 && gifts[38] == 1 && gifts[196] == 1);
  const auto filled_misc = misc;
  const auto filled_gifts = gifts;
  for (const auto& category : NAMED_ITEM_ID_SET_LIST) {
    if (category.storage == ItemStorage::Misc) {
      for (const auto id : category.itemIdSet)
        assert(id < misc.size() && misc[id] == 99);
      fillItemCounts(misc, category.itemIdSet, true);
    } else if (category.storage == ItemStorage::Gifts) {
      for (const auto id : category.itemIdSet)
        assert(id < gifts.size() && gifts[id] == 99);
      fillItemCounts(gifts, category.itemIdSet, true);
    }
  }
  assert(misc == filled_misc && gifts == filled_gifts);
  assert(NAMED_ITEM_ID_SET_LIST.back().name == "Trade Secret");

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
