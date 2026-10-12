// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays.
// Modifications Copyright (c) 2026 Jing Haihan

#pragma once

class ItemTrainerGui final : public MenuGui {
public:
  explicit ItemTrainerGui(Model& model)
    : MenuGui(model, "Item Trainer") {}

protected:
  void populate(tsl::elm::List* list) override {
    list->addItem(
      action_item(model_, text(model_, "Refill All Durability"), [] {
        game::refillItemDurability();
      })
    );
    list->addItem(action_item(model_, text(model_, "Amount to 99"), [] {
      game::fillOwnedItemAmounts();
    }));

    for (const auto& category : core::NAMED_ITEM_ID_SET_LIST) {
      list->addItem(
        action_item(model_, text(model_, category.name) + " ×99", [category] {
          game::fillItemCategory(category, true);
        })
      );
    }
  }
};
