// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays.
// Modifications Copyright (c) 2026 Jing Haihan

#pragma once

class SetItemGui final : public MenuGui {
public:
  explicit SetItemGui(Model& model)
    : MenuGui(model, "Set Item") {}

  void update() override {
    if (name_item_ == nullptr) {
      return;
    }
    const auto id = model_.items().id;
    if (id != last_item_id_) {
      name_item_->setText(
        core::item_name(static_cast<core::ItemId>(id), model_.display_locale())
      );
      const auto maximum =
        core::normal_item_durability(static_cast<core::ItemId>(id));
      if (maximum)
        model_.items().durability = *maximum;
      durability_item_->setMaximum(maximum.value_or(255));
      last_item_id_ = id;
    }
  }

protected:
  void populate(tsl::elm::List* list) override {
    auto& settings = model_.items();
    list->addItem(numeric_item(
      text(model_, "Item ID"), settings.id, {0, core::MAX_ITEM_ID, 100}
    ));
    name_item_ = new tsl::elm::ListItem(
      core::item_name(
        static_cast<core::ItemId>(settings.id), model_.display_locale()
      )
    );
    list->addItem(name_item_);

    durability_item_ = numeric_item(
      text(model_, "Durability"),
      settings.durability,
      {0,
       core::normal_item_durability(static_cast<core::ItemId>(settings.id))
         .value_or(255),
       10}
    );
    list->addItem(durability_item_);
    last_item_id_ = settings.id;
    list->addItem(numeric_item(
      text(model_, "Amount"), settings.amount, {1, core::MAX_ITEM_AMOUNT, 10}
    ));

    list->addItem(action_item(model_, text(model_, "Apply Item"), [this] {
      const auto& settings = model_.items();
      const std::set<core::ItemId> ids{static_cast<core::ItemId>(settings.id)};
      const auto durability =
        static_cast<core::ItemDurability>(settings.durability);
      const auto amount = static_cast<core::ItemAmount>(settings.amount);
      game::setItemsWithIdSet(
        {.ids = &ids,
         .durability = &durability,
         .amount = &amount,
         .addMissing = true}
      );
    }));
  }

private:
  tsl::elm::ListItem* name_item_{};
  NumericListItem* durability_item_{};
  int last_item_id_{-1};
};

class ItemTrainerGui final : public MenuGui {
public:
  explicit ItemTrainerGui(Model& model)
    : MenuGui(model, "Item Trainer") {}

protected:
  void populate(tsl::elm::List* list) override {
    list->addItem(
      submenu_item<SetItemGui>(model_, text(model_, "Set Specific Item"))
    );
    list->addItem(
      action_item(model_, text(model_, "Refill All Durability"), [] {
        game::refillItemDurability();
      })
    );
    category_header(list, text(model_, "Quick Edit"));

    auto& settings = model_.items();
    auto* add_missing = new tsl::elm::ToggleListItem(
      text(model_, "Add Missing Items"),
      settings.addMissing,
      text(model_, "On"),
      text(model_, "Off")
    );
    add_missing->setStateChangedListener([this](bool enabled) {
      model_.items().addMissing = enabled;
    });
    list->addItem(add_missing);

    auto* durability = new tsl::elm::ToggleListItem(
      text(model_, "Normal Durability"),
      settings.normalDurability,
      text(model_, "On"),
      text(model_, "Off")
    );
    durability->setStateChangedListener([this](bool enabled) {
      model_.items().normalDurability = enabled;
    });
    list->addItem(durability);

    auto* amount = new tsl::elm::ToggleListItem(
      text(model_, "Amount to 99"),
      settings.maxAmount,
      text(model_, "On"),
      text(model_, "Off")
    );
    amount->setStateChangedListener([this](bool enabled) {
      model_.items().maxAmount = enabled;
    });
    list->addItem(amount);

    for (const auto& category : core::NAMED_ITEM_ID_SET_LIST) {
      list->addItem(action_item(
        model_, text(model_, category.name), [this, ids = category.itemIdSet] {
          applyItems(&ids, model_.items().addMissing);
        }
      ));
    }

    list->addItem(action_item(model_, text(model_, "Owned Items"), [this] {
      applyItems(nullptr, false);
    }));
  }

private:
  void applyItems(const std::set<core::ItemId>* ids, bool addMissing) {
    const auto& settings = model_.items();
    game::setItemsWithIdSet(
      {.ids = ids,
       .amount = settings.maxAmount ? &core::MAX_ITEM_AMOUNT : nullptr,
       .addMissing = addMissing,
       .normalDurability = settings.normalDurability}
    );
  }
};
