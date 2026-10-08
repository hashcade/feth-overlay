// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays; see LICENSE.

#pragma once

#include "feth/core/types.hpp"

#include <set>
#include <unordered_map>

namespace feth::core {

struct NamedItemIdSet {
  std::string name;
  std::set<ItemId> itemIdSet;
};

struct NamedClassId {
  ClassId id;
  std::string name;
};

struct NameClassIdList {
  std::string name;
  std::list<NamedClassId> list;
};

extern const std::list<NamedItemIdSet> NAMED_ITEM_ID_SET_LIST;
extern const std::list<NameClassIdList> NAMED_CLASS_ID_LIST_LIST;
extern const std::unordered_map<CharacterId, std::string> UNIT_ID_NAME_MAP;
extern const std::list<SupportPair> SUPPORT_LIST;

}  // namespace feth::core
