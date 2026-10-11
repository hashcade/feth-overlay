// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

class OwnedItemGui final : public MenuGui {
public:
  OwnedItemGui(Model& model, std::size_t index)
    : MenuGui(model, "Set Item"),
      index_(index) {}

  void update() override {
    if (!name_item_ || id_ == shown_id_)
      return;
    shown_id_ = id_;
    name_item_->setText(
      core::item_name(static_cast<core::ItemId>(id_), model_.display_locale())
    );
    const auto maximum = maximum_durability();
    durability_ = maximum;
    durability_item_->setMaximum(maximum);
  }

protected:
  void populate(tsl::elm::List* list) override {
    const auto item = game::getConvoyItem(index_);
    id_ = shown_id_ = item.id;
    durability_ = item.durability;
    amount_ = item.amount;

    list->addItem(
      numeric_item(text(model_, "Item ID"), id_, {0, core::MAX_ITEM_ID, 100})
    );
    name_item_ = new tsl::elm::ListItem(
      core::item_name(item.id, model_.display_locale())
    );
    list->addItem(name_item_);

    // Keep existing custom durability until the user changes or refills it.
    durability_item_ = numeric_item(
      text(model_, "Durability"),
      durability_,
      {0, std::max(durability_, maximum_durability()), 10}
    );
    list->addItem(durability_item_);
    list->addItem(numeric_item(
      text(model_, "Amount"), amount_, {1, core::MAX_ITEM_AMOUNT, 10}
    ));

    list->addItem(action_item(model_, text(model_, "Refill Durability"), [this] {
      if (const auto maximum =
            core::normal_item_durability(static_cast<core::ItemId>(id_))) {
        durability_ = *maximum;
        durability_item_->setMaximum(durability_);
        apply();
      }
    }));
    list->addItem(action_item(model_, text(model_, "Apply Item"), [this] {
      apply();
    }));
  }

private:
  void apply() {
    game::setConvoyItem(
      index_,
      {static_cast<core::ItemId>(id_),
       static_cast<core::ItemDurability>(durability_),
       static_cast<core::ItemAmount>(amount_)}
    );
  }

  int maximum_durability() const {
    return core::normal_item_durability(static_cast<core::ItemId>(id_))
      .value_or(255);
  }

  std::size_t index_;
  int id_{};
  int shown_id_{};
  int durability_{};
  int amount_{};
  tsl::elm::ListItem* name_item_{};
  NumericListItem* durability_item_{};
};

class OwnedItemsGui final : public MenuGui {
public:
  explicit OwnedItemsGui(Model& model)
    : MenuGui(model, "Owned Items") {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto items = game::getConvoyItems();
    for (std::size_t index = 0; index < items.size(); ++index) {
      const auto label =
        std::to_string(index + 1) + " · " +
        core::item_name(items[index].id, model_.display_locale());
      list->addItem(submenu_item<OwnedItemGui>(model_, label, index));
    }
  }
};
