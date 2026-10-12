// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays.
// Modifications Copyright (c) 2026 Jing Haihan

#pragma once

class SupportListGui final : public MenuGui {
public:
  SupportListGui(Model& model, core::SupportList entries)
    : MenuGui(
        model,
        core::character_name(entries.displayName, model.display_locale()) +
          " - " + text(model, "Support")
      ),
      entries_(std::move(entries)) {}

protected:
  void populate(tsl::elm::List* list) override {
    items_.clear();
    list->addItem(action_item(model_, text(model_, "Max All"), [this] {
      game::reachMaxSupportRanks(entries_);
      for (const auto& [index, item] : items_)
        refresh_item(index, item);
    }));
    category_header(list, text(model_, "←/→ Rank"));

    for (const auto& entry : entries_.list) {
      const auto& pair = *entry->p_pair;
      auto* item = new tsl::elm::ListItem(
        core::character_name(
          entries_.displayName == pair.first ? pair.second : pair.first,
          model_.display_locale()
        )
      );
      refresh_item(entry->index, item);
      item->setClickListener([this, item, index = entry->index](u64 keys) {
        int step{};
        if (keys & HidNpadButton_AnyLeft)
          step = -1;
        else if (keys & HidNpadButton_AnyRight)
          step = 1;
        else
          return false;

        model_.apply([this, item, index, step] {
          const auto before = game::getSupportPointAtIndex(index);
          const auto after = core::step_support_rank(index, before, step);
          if (after != before)
            game::setSupportPointAtIndex(index, after);
          refresh_item(index, item);
        });
        return true;
      });
      list->addItem(item);
      items_.push_back({entry->index, item});
    }
  }

private:
  void refresh_item(std::size_t index, tsl::elm::ListItem* item) {
    const auto rank =
      core::support_rank(index, game::getSupportPointAtIndex(index));
    item->setValue("< " + text(model_, rank.name) + " >");
  }

  core::SupportList entries_;
  std::vector<std::pair<std::size_t, tsl::elm::ListItem*>> items_;
};

class SupportGui final : public MenuGui {
public:
  explicit SupportGui(Model& model)
    : MenuGui(model, "Support Edit") {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto collection = core::getSupportCollection();
    for (const auto& character : collection.supportListList) {
      list->addItem(
        submenu_item<SupportListGui>(
          model_,
          core::character_name(character.displayName, model_.display_locale()),
          character
        )
      );
    }
  }
};
