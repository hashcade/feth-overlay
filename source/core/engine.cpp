// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#include "feth/core/engine.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/items.hpp"
#include "feth/core/supports.hpp"

#include <unordered_map>

namespace feth::core {

void fillConvoyItems(
  ItemArray& items, ItemCount& count, const ItemFillOptions& options
) {
  auto missing = options.ids;
  for (auto& item : std::span(items).first(count)) {
    if (!options.ids.contains(item.id))
      continue;
    missing.erase(item.id);
    if (options.addMissing || item.amount > 0)
      item.amount = MAX_ITEM_AMOUNT;
  }

  if (!options.addMissing)
    return;

  for (const auto id : missing) {
    if (count == TOTAL_ITEM_COUNT)
      break;
    items[count++] = {
      id, normal_item_durability(id).value_or(0), MAX_ITEM_AMOUNT
    };
  }
}

void fillItemCounts(
  std::span<ItemAmount> counts, const std::set<ItemId>& ids, bool addMissing
) {
  for (const auto id : ids) {
    if (addMissing || counts[id] > 0)
      counts[id] = MAX_ITEM_AMOUNT;
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
    const auto pairIndex = index++;
    if (support_rank_mask(pairIndex) == 0)
      continue;

    auto curSupportEntry =
      std::make_shared<SupportEntry>(SupportEntry{pairIndex, &supportPair});
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
  }

  return resultSupportCollection;
}

}  // namespace feth::core
