// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#include "feth/core/engine.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/items.hpp"

#include <map>
#include <unordered_map>

namespace feth::core {

void editItems(
  ItemArray& items, ItemCount& curItemCount, const ItemEditOptions& options
) {
  const auto* p_itemIdSet = options.ids;
  const auto* p_durabilityToSet = options.durability;
  const auto* p_amountToSet = options.amount;
  // make a map to keep track which item was found
  auto foundItemMap = std::map<ItemId, bool>{};
  if (p_itemIdSet) {
    for (auto& itemId : *p_itemIdSet) {
      foundItemMap[itemId] = false;
    }
  }

  // search and set existing items
  for (ItemCount index = 0; index < curItemCount; ++index) {
    auto& item = items[index];
    auto itemEntryInFoundMap = foundItemMap.find(item.id);
    auto itemIsFound = itemEntryInFoundMap != end(foundItemMap);

    if (itemIsFound) {
      itemEntryInFoundMap->second = true;
    }

    if (not p_itemIdSet or itemIsFound) {
      if (p_durabilityToSet)
        item.durability = *p_durabilityToSet;
      if (p_amountToSet)
        item.amount = *p_amountToSet;
      if (options.normalDurability) {
        if (const auto maximum = normal_item_durability(item.id)) {
          item.durability = *maximum;
        }
      }
    }
  }

  // take care of adding items if necessary
  if (p_itemIdSet and options.addMissing) {
    for (auto& foundItemMapEntry : foundItemMap) {
      auto itemId = foundItemMapEntry.first;
      auto itemWasFound = foundItemMapEntry.second;

      if (curItemCount >= TOTAL_ITEM_COUNT) {
        break;
      }

      if (not itemWasFound) {
        auto durabilityToSet = p_durabilityToSet
                                 ? *p_durabilityToSet
                                 : normal_item_durability(itemId).value_or(0);
        auto amountToSet = p_amountToSet ? *p_amountToSet : ItemAmount{1};
        items[curItemCount] = {itemId, durabilityToSet, amountToSet};
        curItemCount++;
      }
    }
  }
}

// helpers
auto classIsUnlocked(const ClassUnlocks& classUnlocksTestedOn, ClassId classId)
  -> bool {
  if (
    classId >= BASE_CLASS_ID_START and
    classId < BASE_CLASS_ID_START + CLASS_UNLOCK_BIT_FIELD_SIZE
  ) {
    return classUnlocksTestedOn.baseClassUnlocks[classId - BASE_CLASS_ID_START];

  } else if (
    classId >= SPECIAL_CLASS_ID_START and
    classId < SPECIAL_CLASS_ID_START + CLASS_UNLOCK_BIT_FIELD_SIZE
  ) {
    return classUnlocksTestedOn
      .specialClassUnlocks[classId - SPECIAL_CLASS_ID_START];
  }

  // nowhere to check these
  return false;
}

SupportCollection getSupportCollection() {
  auto resultSupportCollection = SupportCollection{};
  auto& allList = resultSupportCollection.allList;
  auto& supportListList = resultSupportCollection.supportListList;

  auto characterSupportListMap =
    std::unordered_map<std::string, SupportList*>{};

  auto index = size_t{0};
  for (auto& supportPair : SUPPORT_LIST) {
    auto curSupportEntry =
      std::make_shared<SupportEntry>(SupportEntry{index, &supportPair});
    allList.push_back(curSupportEntry);

    if (
      characterSupportListMap.find(supportPair.first) ==
      end(characterSupportListMap)
    ) {
      supportListList.push_back({supportPair.first, {curSupportEntry}});
      characterSupportListMap[supportPair.first] = &(supportListList.back());
    } else {
      characterSupportListMap[supportPair.first]->list.push_back(
        curSupportEntry
      );
    }

    if (
      characterSupportListMap.find(supportPair.second) ==
      end(characterSupportListMap)
    ) {
      supportListList.push_back({supportPair.second, {curSupportEntry}});
      characterSupportListMap[supportPair.second] = &(supportListList.back());
    } else {
      characterSupportListMap[supportPair.second]->list.push_back(
        curSupportEntry
      );
    }

    index++;
  }

  return resultSupportCollection;
}

}  // namespace feth::core
