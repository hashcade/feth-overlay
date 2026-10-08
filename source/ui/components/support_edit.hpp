// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays.
// Modifications Copyright (c) 2026 Jing Haihan

#pragma once

class SupportEditGui final : public MenuGui {
public:
  SupportEditGui(Model& model, core::SupportEntry entry)
    : MenuGui(
        model,
        core::character_name(entry.p_pair->first, model.display_locale()) +
          " x " +
          core::character_name(entry.p_pair->second, model.display_locale())
      ),
      entry_(entry) {}

protected:
  void populate(tsl::elm::List* list) override {
    points_ = game::getSupportPointAtIndex(entry_.index);
    list->addItem(
      numeric_item(text(model_, "Support Points"), points_, {0, 9999, 100})
    );
    list->addItem(action_item(model_, text(model_, "Apply Support"), [this] {
      game::setSupportPointAtIndex(
        entry_.index, static_cast<core::SupportPoint>(points_)
      );
    }));
  }

private:
  core::SupportEntry entry_;
  int points_{};
};

class SupportListGui final : public MenuGui {
public:
  SupportListGui(Model& model, core::SupportList entries)
    : MenuGui(
        model,
        core::character_name(entries.displayName, model.display_locale()) +
          " - " + text(model, "Support")
      ),
      entries_(std::move(entries)) {}

  void update() override {
    if (pending_item_ == nullptr) {
      return;
    }
    if (model_.refresh()) {
      try {
        pending_item_->setValue(
          std::to_string(game::getSupportPointAtIndex(pending_index_))
        );
      } catch (const std::exception& error) {
        model_.showError(error.what());
      }
    }
    pending_item_ = nullptr;
  }

protected:
  void populate(tsl::elm::List* list) override {
    for (const auto& entry : entries_.list) {
      const auto& pair = *entry->p_pair;
      auto* item = new tsl::elm::ListItem(
        core::character_name(
          entries_.displayName == pair.first ? pair.second : pair.first,
          model_.display_locale()
        )
      );
      item->setValue(
        std::to_string(game::getSupportPointAtIndex(entry->index))
      );
      item->setClickListener([this, item, entry](u64 keys) {
        if ((keys & HidNpadButton_A) == 0) {
          return false;
        }
        pending_item_ = item;
        pending_index_ = entry->index;
        tsl::changeTo<SupportEditGui>(model_, *entry);
        return true;
      });
      list->addItem(item);
    }
  }

private:
  core::SupportList entries_;
  tsl::elm::ListItem* pending_item_{};
  std::size_t pending_index_{};
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
